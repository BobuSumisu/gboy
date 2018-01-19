#ifndef GBOY_MMU_H
#define GBOY_MMU_H

#include <inttypes.h>
#include "gpu.h"
#include "cpu.h"

struct mmu {
    uint8_t reg_boot;           /* 0xFF50 */

    uint8_t rom0[0x4000];       /* 0x0000 - 0x4000 = 16 KiB internal ROM. */
    uint8_t rom1[0x4000];       /* 0x4000 - 0x8000 = 16 KiB switchable ROM. */
                                /* 0x8000 - 0xA000 =  8 KiB video RAM. */
    uint8_t ram1[0x2000];       /* 0xA000 - 0xC000 =  8 KiB switchable RAM. */
    uint8_t ram0[0x2000];       /* 0xC000 - 0xE000 =  8 KiB internal RAM. */
                                /* 0xE000 - 0xFE00 =  7.5 KiB internal RAM shadow. */
                                /* 0xFE00 - 0xFEA0 =  160 B sprite data. */
                                /* 0xFEA0 - 0xFF00 =  96 B empty (unusable?) */
    uint8_t ports[0x80];        /* 0xFF00 - 0xFF80 =  128 B I/O ports. */
    uint8_t zram[0x7F];         /* 0xFF80 - 0xFFFF =  127 B internal RAM. */
                                /* 0xFFFF = IE register. */
    struct cpu *cpu;
    struct gpu *gpu;
};

struct mmu  *mmu_new(struct cpu *cpu, struct gpu *gpu);
void        mmu_init(struct mmu *mmu, struct cpu *cpu, struct gpu *gpu);
void        mmu_free(struct mmu *mmu);
uint8_t     mmu_rb(const struct mmu *mmu, const uint16_t addr);
void        mmu_wb(struct mmu *mmu, const uint16_t addr, const uint8_t b);
uint16_t    mmu_rw(const struct mmu *mmu, const uint16_t addr);
void        mmu_ww(struct mmu *mmu, const uint16_t addr, const uint16_t w);

int         mmu_load_rom(struct mmu *mmu, const char *path);

#endif
