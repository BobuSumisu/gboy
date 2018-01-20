#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "gpu.h"
#include "bitutil.h"
#include "screen.h"
#include "cpu.h"

#define CYCLES_HBLANK 204
#define CYCLES_VBLANK 4560
#define CYCLES_OAM 80
#define CYCLES_RAM 172
#define CYCLES_LINE (CYCLES_OAM + CYCLES_RAM + CYCLES_HBLANK)

/*** Private ***/

static uint8_t get_color(uint8_t data, uint8_t palette) {
    switch(data) {
        case 0: return (palette >> 0) & 0x03;
        case 1: return (palette >> 1) & 0x03;
        case 2: return (palette >> 2) & 0x03;
        case 3: return (palette >> 3) & 0x03;
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

    if(gpu->reg_lcdc & LCDC_WIN_ON) {
        win = gpu->reg_wy <= gpu->reg_ly;
    }

    if(win) {
        printf("win!\n");
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
        gpu->screen->back_buffer[rgb_index + 4] = 0xFF;
    }
}

static void gpu_scanline_objects(struct gpu *gpu) {
    (void)gpu;
    printf("objects\n");
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

void gpu_init(struct gpu *gpu, struct cpu *cpu, struct screen *screen) {
    memset(gpu, 0, sizeof(struct gpu));
    gpu->cpu = cpu;
    gpu->screen = screen;
}

void gpu_cleanup(struct gpu *gpu) {
    (void)gpu;
}

void gpu_update(struct gpu *gpu, int cycles) {

    /* Check that LCD is turned on. */
    if(!(gpu->reg_lcdc & LCDC_ON)) {
        return;
    }

    gpu->clock += cycles;

    /* Check if should switch mode (and reset clock). */
    enum gpu_mode new_mode = gpu->mode;
    switch(gpu->mode) {
        case GPU_MODE_HBLANK:
            if(gpu->clock >= CYCLES_HBLANK) {
                gpu->clock -= CYCLES_HBLANK;
                gpu->reg_ly++;
                if(gpu->reg_ly == 144) {
                    new_mode = GPU_MODE_VBLANK;
                } else {
                    new_mode = GPU_MODE_OAM;
                }
            }
            break;
        case GPU_MODE_VBLANK:
            /* 10 lines */
            if(gpu->clock >= CYCLES_LINE) {
                gpu->clock -= CYCLES_LINE;
                gpu->reg_ly++;
                if(gpu->reg_ly == 154) {
                    gpu->reg_ly = 0;
                    new_mode = GPU_MODE_OAM;
                }
            }
            break;
        case GPU_MODE_OAM:
            if(gpu->clock >= CYCLES_OAM) {
                gpu->clock -= CYCLES_OAM;
                new_mode = GPU_MODE_RAM;
            }
            break;
        case GPU_MODE_RAM:
            if(gpu->clock >= CYCLES_RAM) {
                gpu->clock -= CYCLES_RAM;
                new_mode = GPU_MODE_HBLANK;
            }
            break;
    }

    /* Check if LY == LYC and interrupt if enabled. */
    if(gpu->reg_ly == gpu->reg_lyc) {
        gpu->reg_stat |= STAT_MATCH;
        if(gpu->reg_stat & STAT_INT_MATCH) {
            gpu->cpu->reg_if |= INT_LCDC;
        }
    }

    if(new_mode != gpu->mode) {
        /* Write current mode to STAT_MODE */
        gpu->reg_stat = (gpu->reg_stat & 0xFC) | gpu->mode;

        gpu->mode = new_mode;
        switch(new_mode) {
            case GPU_MODE_HBLANK:
                if(gpu->reg_stat & STAT_INT_HBLANK) {
                    gpu->cpu->reg_if |= INT_LCDC;
                }
                break;
            case GPU_MODE_VBLANK:
                if(gpu->reg_stat & STAT_INT_VBLANK) {
                    gpu->cpu->reg_if |= INT_LCDC;
                }
                gpu->cpu->reg_if |= INT_VBLANK;
                break;
            case GPU_MODE_OAM:
                if(gpu->reg_stat & STAT_INT_OAM) {
                    gpu->cpu->reg_if |= INT_LCDC;
                }
                break;
            case GPU_MODE_RAM:
                gpu_scanline(gpu);
                break;
        }
    }
}

void gpu_debug_tiles(struct gpu *gpu) {

    printf("Tile Palette Data (BGP 0xFF47):\n");
    printf("00 => %d\n", (gpu->reg_bgp >> 0) & 0x3);
    printf("01 => %d\n", (gpu->reg_bgp >> 2) & 0x3);
    printf("10 => %d\n", (gpu->reg_bgp >> 4) & 0x3);
    printf("11 => %d\n", (gpu->reg_bgp >> 6) & 0x3);

    printf("Tile Data #1 (0x8000 - 0x9000):\n");
    for(int i = 0; i < 128; i++) {
        for(int row = 0; row < 8; row++) {
            // for(int col = 7; col >= 0; col--) {
            for(int col = 0; col < 8; col++) {
                int d = tile_data_pixel((struct tile_data *)&gpu->vram[0], i, row, col);
                int c = get_color((uint8_t)d, gpu->reg_bgp);
                switch(c) {
                    case 0: printf("."); break;
                    default: printf("O"); break;
                }
            }
            printf("\n");
        }
        printf("\n");
    }
    printf("-----");

    for(int i = 0; i < 128; i++) {
        uint16_t tile_addr = (uint16_t)(0x8000 + (i * 16)) & 0x1FFF;

        for(int row = 0; row < 8; row++) {
            uint8_t data_a = gpu->vram[tile_addr + (row * 2) + 0];
            uint8_t data_b = gpu->vram[tile_addr + (row * 2) + 1];

            for(int col = 7; col >= 0; col--) {
                // uint8_t data = (uint8_t)(((data_a >> col) & 1) | (((data_b >> col) & 1)));
                int data = BIT_GET(data_a, col) | BIT_GET(data_b, col) << 1;
                uint8_t color = (uint8_t)get_color((uint8_t)data, gpu->reg_bgp);
                switch(color) {
                    case 0: printf("."); break;
                    case 1: printf("*"); break;
                    case 2: printf("o"); break;
                    case 3: printf("O"); break;
                    default: printf("ERROR: data = %d\n", data); return;
                }
            }
            printf("\n");
        }
        printf("\n");
    }

    printf("Tile Data #0 (0x8800 - 0x9800):\n");

    printf("Background Tile Map #1 (0x9800):\n");
    for(uint16_t y = 0; y < 32; y++) {
        for(uint16_t x = 0; x < 32; x++) {
            uint8_t tile = gpu->vram[(0x9800 + ((y * 32) + x)) & 0x1FFF];
            if(tile != 0) {
                printf("%02X ", tile);
            } else {
                printf("-- ");
            }
        }
        printf("\n");
    }

    printf("Background Tile Map #2 (0x9C00):\n");
    for(uint16_t y = 0; y < 32; y++) {
        for(uint16_t x = 0; x < 32; x++) {
            uint8_t tile = gpu->vram[(0x9C00 + ((y * 32) + x)) & 0x1FFF];
            if(tile != 0) {
                printf("%02X ", (int8_t)tile);
            } else {
                printf("-- ");
            }
        }
        printf("\n");
    }
}
