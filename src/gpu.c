/*
 *  The VRAM:
 *  | Addr   | 7 6 5 4 3 2 1 0 |
 *  | 0x8000 |                 | Character Data 1
 *  | 0x8800 |                 | Character Data 2
 *  | 0x9800 |                 | BG Display Data 1
 *  | 0x9C00 |                 | BG Display Data 2
 *
 *  Two screens of BG (selected by LCDC_BG_CHAR).
 *  1 byte CHR code.
 *
 *  Data for 32x32 char codes (256x256 dots).
 *  Only 20x18 char codes (160x144 dots) are displayed.
 *
 *  +----+----+----+-----+----+----+----+
 *  |  0 |  1 |  2 | ... | 29 | 30 | 31 |
 *  +----+----+----+-----+----+----+----+
 *  | 32 | 33 | 34 | ... | 61 | 62 | 63 |
 *  +----+----+----+-----+----+----+----+
 *  |    |    |    | ... |    |    |    |
 *  +----+----+----+-----+----+----+----+
 *
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "gpu.h"

/*** Private ***/

struct obj_data {
    uint8_t y;
    uint8_t x;
    uint8_t char_code;
    uint8_t attr;
};

static void gpu_scanline_background(struct gpu *gpu) {
    (void)gpu;

    uint8_t screen_y = gpu->reg_ly + gpu->reg_scy;
    uint16_t char_addr = (gpu->reg_lcdc & LCDC_BG_CHAR) ? 0x8800 : 0x0800;

    for(uint8_t screen_x = 0; screen_x < 160; screen_x++) {
        uint8_t array_x = screen_x / 32;
        uint8_t array_y = screen_y / 32;
        uint8_t char_code = gpu->vram[(char_addr + (array_y * 32) + array_x) & 0x1FFF];
        if(char_code != 0) {
            printf("(%d, %d) = %d\n", screen_x, screen_x, char_code);
        }
    }
}

static void gpu_scanline_objects(struct gpu *gpu) {
    (void)gpu;
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

struct gpu *gpu_new(struct cpu *cpu) {
    struct gpu *gpu = malloc(sizeof(struct gpu));
    gpu_init(gpu, cpu);
    return gpu;
}

void gpu_init(struct gpu *gpu, struct cpu *cpu) {
    memset(gpu, 0, sizeof(struct gpu));
    gpu->cpu = cpu;
}

void gpu_free(struct gpu *gpu) {
    free(gpu);
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
            if(gpu->clock >= 51) {
                gpu->clock -= 51;
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
            if(gpu->clock >= 114) {
                gpu->clock -= 114;
                gpu->reg_ly++;
                if(gpu->reg_ly == 154) {
                    gpu->reg_ly = 0;
                    new_mode = GPU_MODE_OAM;
                }
            }
            break;
        case GPU_MODE_OAM:
            if(gpu->clock >= 20) {
                gpu->clock -= 20;
                new_mode = GPU_MODE_RAM;
            }
            break;
        case GPU_MODE_RAM:
            if(gpu->clock >= 43) {
                gpu->clock -= 43;
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
