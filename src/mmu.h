#ifndef GBOY_MMU_H
#define GBOY_MMU_H

#include <inttypes.h>

struct mmu {
    int reg_boot;

    uint8_t rom0[0x4000];   /* 0x0000 - 0x3FFF = 16 KiB internal ROM. */
    uint8_t rom1[0x4000];   /* 0x4000 - 0x7FFF = 16 KiB switchable ROM. */
    uint8_t vram[0x2000];   /* 0x8000 - 0x9FFF =  8 KiB video RAM. */
    uint8_t ram1[0x2000];   /* 0xA000 - 0xBFFF =  8 KiB switchable RAM. */
    uint8_t ram0[0x2000];   /* 0xC000 - 0xDFFF =  8 KiB internal RAM. */
                            /* 0xE000 - 0xFDFF =  7.5 KiB internal RAM shadow. */
    uint8_t oam[0xA0];      /* 0xFE00 - 0xFE9F =  160 B sprite data. */
                            /* 0xFEA0 - 0xFEFF =  96 B empty (unusable?) */
    uint8_t ports[0x80];    /* 0xFF00 - 0xFF7F =  128 B I/O ports. */
    uint8_t zram[0x7F];     /* 0xFF80 - 0xFFFF =  127 B internal RAM. */
                            /* 0xFFFF = IE register. */
};

void        mmu_init(struct mmu *mmu);
void        mmu_free(struct mmu *mmu);
uint8_t     mmu_rb(const struct mmu *mmu, const uint16_t addr);
void        mmu_wb(struct mmu *mmu, const uint16_t addr, const uint8_t b);
uint16_t    mmu_rw(const struct mmu *mmu, const uint16_t addr);
void        mmu_ww(struct mmu *mmu, const uint16_t addr, const uint16_t w);

#endif
