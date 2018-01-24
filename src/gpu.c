#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "gpu.h"
#include "interrupt.h"
#include "bitutil.h"
#include "screen.h"

#define CYCLES_HBLANK   204
#define CYCLES_VBLANK   4560    /* 10 lines */
#define CYCLES_OAM      80
#define CYCLES_RAM      172
#define CYCLES_LINE     456     /* OAM+RAM+HBLANK */

/*** Private ***/

static uint8_t get_color(uint8_t data, uint8_t palette) {
    switch(data) {
        case 0: return (palette >> 0) & 0x03;
        case 1: return (palette >> 2) & 0x03;
        case 2: return (palette >> 4) & 0x03;
        case 3: return (palette >> 6) & 0x03;
    }
    return 0;
}

static uint8_t get_color_value(uint8_t color) {
    switch(color) {
        case 0: return 0xFF;
        case 1: return 0xC0;
        case 2: return 0x60;
        case 3: return 0x00;
    }
    return 0xFF;
}

#define OBJ_ATTR_PRIORITY       0x80
#define OBJ_ATTR_VFLIP          0x40
#define OBJ_ATTR_HFLIP          0x20
#define OBJ_ATTR_PALETTE        0x10
#define OBJ_ATTR_BANK_CGB       0x08
#define OBJ_ATTR_PALETTE_CGB    0x07

struct obj {
    uint8_t y;
    uint8_t x;
    uint8_t tile_id;
    uint8_t attr;
} __attribute__ ((packed));

/* Make it a little easier to work with tile pixel data. */
struct tile_data {
    struct {
        uint8_t lo;
        uint8_t hi;
    } __attribute__ ((packed)) rows[8];
} __attribute__ ((packed));

/* Get the color for specific pixel in tile i in the data. */
static uint8_t tile_data_pixel(const struct tile_data *data, const int i, const int y, const int x) {
    /* Pixel rows are reversed. */
    int rx = 7 - x;
    return (uint8_t)(BIT_GET(data[i].rows[y].lo, rx) | BIT_GET(data[i].rows[y].hi, rx) << 1);
}

static void gpu_scanline_background(struct gpu *gpu) {
    int win = 0;
    int signed_ids = (gpu->reg_lcdc & LCDC_BG_DATA) == 0;

    if(gpu->reg_lcdc & LCDC_WIN_ON) {
        win = gpu->reg_wy <= gpu->reg_ly;
    }

    if(win) {
        printf("window not implemented!\n");
        return;
    }

    uint8_t sy = gpu->reg_ly + gpu->reg_scy;
    uint8_t *map_data = (gpu->reg_lcdc & LCDC_BG_MAP) ? &gpu->vram[0x1C00] : &gpu->vram[0x1800];
    uint8_t *tile_data = (gpu->reg_lcdc & LCDC_BG_DATA) ? &gpu->vram[0x0000] : &gpu->vram[0x0800];

    for(uint8_t i = 0; i < SCREEN_WIDTH; i++) {
        uint8_t sx = i + gpu->reg_scx;

        /* Convert from screen y, x to map y, x. */
        uint8_t my = sy / 8;
        uint8_t mx = sx / 8;

        /* Get the tile id. */
        uint8_t tile_id = map_data[(my * 32) + mx];

        if(signed_ids) {
            tile_id = (uint8_t)((int8_t)(tile_id + 128));
        }

        /* And pixel y, x. */
        uint8_t py = sy % 8;
        uint8_t px = sx % 8;

        /* Use the tile id and pixel coordinates to get color for current point. */
        uint8_t data = tile_data_pixel((struct tile_data *)tile_data, tile_id, py, px);
        uint8_t color = get_color(data, gpu->reg_bgp);

        /* Convert color to screen color. */
        uint8_t value = get_color_value(color);

        /* And set it in the screen back buffer. */
        int rgb_index = ((gpu->reg_ly * SCREEN_WIDTH) + i) * 4;
        gpu->screen->back_buffer[rgb_index + 0] = value;
        gpu->screen->back_buffer[rgb_index + 1] = value;
        gpu->screen->back_buffer[rgb_index + 2] = value;
        gpu->screen->back_buffer[rgb_index + 3] = 0xFF;
    }
}

static void gpu_scanline_objects(struct gpu *gpu) {
    /* TODO: sprite overlapping, priority */
    struct obj *obj;
    uint8_t h = (gpu->reg_lcdc & LCDC_OBJ_SIZE) ? 16 : 8;
    for(int i = 0; i < 40; i++) {
        obj = (struct obj *)&gpu->oam[i * 4];
        uint8_t y = obj->y - 16;
        uint8_t x = obj->x - 8;

        if(gpu->reg_ly >= y && gpu->reg_ly < (y + h)) {
            uint8_t py = gpu->reg_ly - y;
            if(obj->attr & OBJ_ATTR_VFLIP) {
                py = h - py;
            }

            for(uint8_t px = 0; px < 8; px++) {
                uint8_t data;
                if(obj->attr & OBJ_ATTR_HFLIP) {
                    data = tile_data_pixel((struct tile_data *)&gpu->vram[0], obj->tile_id, py, 8 - px);
                } else {
                    data = tile_data_pixel((struct tile_data *)&gpu->vram[0], obj->tile_id, py, px);
                }

                if(data == 0) {
                    continue;
                }

                uint8_t color = get_color(data, (obj->attr & OBJ_ATTR_PALETTE) ? gpu->reg_obp1 : gpu->reg_obp0);

                if(obj->attr & OBJ_ATTR_PRIORITY) {
                    /* TODO: priority */
                }

                uint8_t value = get_color_value(color);
                int rgb_index = ((gpu->reg_ly * SCREEN_WIDTH) + x + px) * 4;
                gpu->screen->back_buffer[rgb_index + 0] = value;
                gpu->screen->back_buffer[rgb_index + 1] = value;
                gpu->screen->back_buffer[rgb_index + 2] = value;
                gpu->screen->back_buffer[rgb_index + 3] = 0xFF;
            }
        }
    }
}

static void gpu_scanline(struct gpu *gpu) {
    if(gpu->reg_lcdc & LCDC_BG_ON) {
        gpu_scanline_background(gpu);
    }

    if(gpu->reg_lcdc & LCDC_OBJ_ON) {
        gpu_scanline_objects(gpu);
    }
}

/*** Public ***/

void gpu_init(struct gpu *gpu, struct interrupt_controller *ic, struct screen *screen) {
    memset(gpu, 0, sizeof(struct gpu));
    gpu->ic = ic;
    gpu->screen = screen;
    gpu->mode = GPU_MODE_OAM;
}

void gpu_cleanup(struct gpu *gpu) {
    (void)gpu;
}

void gpu_update(struct gpu *gpu, int cycles) {
    assert(gpu->reg_ly < 154);

    /* Update MODE in STAT. */
    if((gpu->reg_stat & 3) != gpu->mode) {
        gpu->reg_stat = (gpu->reg_stat & 0xFC) | gpu->mode;
    }

    /* Check that LCD is turned on. */
    if((gpu->reg_lcdc & LCDC_ON) == 0) {
        gpu->clock = 0;
        // gpu->mode = GPU_MODE_OAM; Dr. Mario hangs if not 0?
        gpu->mode = 0;
        gpu->reg_ly = 0;
        return;
    }

    /* Set MATCH field in STAT and interrupt if enabled. */
    if(gpu->reg_ly == gpu->reg_lyc) {
        gpu->reg_stat |= STAT_MATCH;
        if(gpu->reg_stat & STAT_INT_MATCH) {
            interrupt_controller_trigger(gpu->ic, INT_LCDC);
        }
    } else {
        gpu->reg_stat &= ~STAT_MATCH;
    }

    gpu->clock += cycles;

    /* Check if we should switch mode and. */
    switch(gpu->mode) {
        case GPU_MODE_OAM:
            if(gpu->clock >= CYCLES_OAM) {
                /* Switch OAM -> RAM */
                gpu->clock %= CYCLES_OAM;
                gpu->mode = GPU_MODE_RAM;
            }
            break;
        case GPU_MODE_RAM:
            if(gpu->clock >= CYCLES_RAM) {
                /* Switch RAM -> HBLANK */
                gpu->clock %= CYCLES_RAM;
                gpu->mode = GPU_MODE_HBLANK;

                /* Do at scanline now. */
                gpu_scanline(gpu);

                /* STAT HBLANK interrupt. */
                if(gpu->reg_stat & STAT_INT_HBLANK) {
                    interrupt_controller_trigger(gpu->ic, INT_LCDC);
                }
            }
            break;
        case GPU_MODE_HBLANK:
            if(gpu->clock >= CYCLES_HBLANK) {
                gpu->clock %= CYCLES_HBLANK;
                gpu->reg_ly++;
                if(gpu->reg_ly == 144) {
                    /* Switch HBLANK -> VBLANK */
                    gpu->mode = GPU_MODE_VBLANK;

                    /* VBLANK interrupt */
                    interrupt_controller_trigger(gpu->ic, INT_VBLANK);

                    /* STAT VBLANK interrupt */
                    if(gpu->reg_stat & STAT_INT_VBLANK) {
                        interrupt_controller_trigger(gpu->ic, INT_LCDC);
                    }
                } else {
                    /* Switch HBLANK -> OAM */
                    gpu->mode = GPU_MODE_OAM;

                    /* STAT OAM interrupt */
                    if(gpu->reg_stat & STAT_INT_OAM) {
                        interrupt_controller_trigger(gpu->ic, INT_LCDC);
                    }
                }
            }
            break;
        case GPU_MODE_VBLANK:
            /* VBLANK is 10 lines (OAM+RAM+HBLANK). */
            if(gpu->clock >= CYCLES_LINE) {
                gpu->clock %= CYCLES_LINE;
                gpu->reg_ly++;
                if(gpu->reg_ly == 154) {
                    /* Switch VBLANK -> OAM */
                    gpu->reg_ly = 0;
                    gpu->mode = GPU_MODE_OAM;

                    /* STAT OAM interrupt. */
                    if(gpu->reg_stat & STAT_INT_OAM) {
                        interrupt_controller_trigger(gpu->ic, INT_LCDC);
                    }
                }
            }
            break;
    }
}

uint8_t gpu_io_lcdc(const struct gpu *gpu) {
    return gpu->reg_lcdc;
}

uint8_t gpu_io_stat(const struct gpu *gpu) {
    return gpu->reg_stat;
}

uint8_t gpu_io_scy(const struct gpu *gpu) {
    return gpu->reg_scy;
}

uint8_t gpu_io_scx(const struct gpu *gpu) {
    return gpu->reg_scx;
}

uint8_t gpu_io_ly(const struct gpu *gpu) {
    return gpu->reg_ly;
}

uint8_t gpu_io_lyc(const struct gpu *gpu) {
    return gpu->reg_lyc;
}

uint8_t gpu_io_dma(const struct gpu *gpu) {
    return gpu->reg_dma;
}

uint8_t gpu_io_bgp(const struct gpu *gpu) {
    return gpu->reg_bgp;
}

uint8_t gpu_io_obp0(const struct gpu *gpu) {
    return gpu->reg_obp0;
}

uint8_t gpu_io_obp1(const struct gpu *gpu) {
    return gpu->reg_obp1;
}

uint8_t gpu_io_wy(const struct gpu *gpu) {
    return gpu->reg_wy;
}

uint8_t gpu_io_wx(const struct gpu *gpu) {
    return gpu->reg_wx;
}

void gpu_io_set_lcdc(struct gpu *gpu, const uint8_t v) {
    gpu->reg_lcdc = v;
}

void gpu_io_set_stat(struct gpu *gpu, const uint8_t v) {
    gpu->reg_stat = v & 0x7C;
}

void gpu_io_set_scy(struct gpu *gpu, const uint8_t v) {
    gpu->reg_scy = v;
}

void gpu_io_set_scx(struct gpu *gpu, const uint8_t v) {
    gpu->reg_scx = v;
}

void gpu_io_set_ly(struct gpu *gpu, const uint8_t v) {
    (void)v;
    gpu->reg_ly = 0;
}

void gpu_io_set_lyc(struct gpu *gpu, const uint8_t v) {
    gpu->reg_lyc = v;
}

void gpu_io_set_dma(struct gpu *gpu, const uint8_t v) {
    gpu->reg_dma = v;
}

void gpu_io_set_bgp(struct gpu *gpu, const uint8_t v) {
    gpu->reg_bgp = v;
}

void gpu_io_set_obp0(struct gpu *gpu, const uint8_t v) {
    gpu->reg_obp0 = v;
}

void gpu_io_set_obp1(struct gpu *gpu, const uint8_t v) {
    gpu->reg_obp1 = v;
}

void gpu_io_set_wy(struct gpu *gpu, const uint8_t v) {
    gpu->reg_wy = v;
}

void gpu_io_set_wx(struct gpu *gpu, const uint8_t v) {
    gpu->reg_wx = v;
}
