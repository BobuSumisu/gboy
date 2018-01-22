/*
 *  gpu.h
 *  =====
 *
 *  Emulate the Game Boy LCD controller.
 *
 *  A lot of confusing stuff here. :)
 *
 */
#ifndef GBOY_GPU_H
#define GBOY_GPU_H

#include <inttypes.h>

#define LCDC_ON             0x80
#define LCDC_WIN_MAP        0x40
#define LCDC_WIN_ON         0x20
#define LCDC_BG_DATA        0x10
#define LCDC_BG_MAP         0x08
#define LCDC_OBJ_SIZE       0x04
#define LCDC_OBJ_ON         0x02
#define LCDC_BG_ON          0x01

#define STAT_INT_MATCH      0x40
#define STAT_INT_OAM        0x20
#define STAT_INT_VBLANK     0x10
#define STAT_INT_HBLANK     0x08
#define STAT_MATCH          0x04 /* Write to MATCH set it to 0 */
#define STAT_MODE           0x03

enum gpu_mode {
    GPU_MODE_HBLANK,        /* Mode 00 - CPU has access to VRAM */
    GPU_MODE_VBLANK,        /* Mode 01 - CPU has access to VRAM */
    GPU_MODE_OAM,           /* Mode 10 - OAM in use */
    GPU_MODE_RAM,           /* Mode 11 - OAM and VRAM in use */
};

struct gpu {
    uint8_t reg_lcdc;       /* 0xFF40 */
    uint8_t reg_stat;       /* 0xFF41 */
    uint8_t reg_scy;        /* 0xFF42 */
    uint8_t reg_scx;        /* 0xFF43 */
    uint8_t reg_ly;         /* 0xFF44 */
    uint8_t reg_lyc;        /* 0xFF45 */
    uint8_t reg_dma;        /* 0xFF46 */
    uint8_t reg_bgp;        /* 0xFF47 */
    uint8_t reg_obp0;       /* 0xFF48 */
    uint8_t reg_obp1;       /* 0xFF49 */
    uint8_t reg_wy;         /* 0xFF4A */
    uint8_t reg_wx;         /* 0xFF4B */

    uint8_t vram[0x2000];   /* 0x8000 - 0xA000 */
    uint8_t oam[0xA0];      /* 0xFE00 - 0xFEA0 */
    enum gpu_mode mode;
    int clock;

    struct interrupts *interrupts;
    struct screen *screen;
};

void gpu_init(struct gpu *gpu, struct interrupts *interrupts, struct screen *screen);
void gpu_cleanup(struct gpu *gpu);
void gpu_update(struct gpu *gpu, int cycles);
void gpu_debug_tiles(struct gpu *gpu);

#endif
