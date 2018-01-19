#include <stdlib.h>
#include <string.h>
#include "mmu.h"

/*** Private ***/

static const uint8_t boot[] = {
    0x31, 0xfe, 0xff, 0xaf, 0x21, 0xff, 0x9f, 0x32, 0xcb, 0x7c, 0x20, 0xfb, 0x21, 0x26, 0xff, 0x0e,
    0x11, 0x3e, 0x80, 0x32, 0xe2, 0x0c, 0x3e, 0xf3, 0xe2, 0x32, 0x3e, 0x77, 0x77, 0x3e, 0xfc, 0xe0,
    0x47, 0x11, 0x04, 0x01, 0x21, 0x10, 0x80, 0x1a, 0xcd, 0x95, 0x00, 0xcd, 0x96, 0x00, 0x13, 0x7b,
    0xfe, 0x34, 0x20, 0xf3, 0x11, 0xd8, 0x00, 0x06, 0x08, 0x1a, 0x13, 0x22, 0x23, 0x05, 0x20, 0xf9,
    0x3e, 0x19, 0xea, 0x10, 0x99, 0x21, 0x2f, 0x99, 0x0e, 0x0c, 0x3d, 0x28, 0x08, 0x32, 0x0d, 0x20,
    0xf9, 0x2e, 0x0f, 0x18, 0xf3, 0x67, 0x3e, 0x64, 0x57, 0xe0, 0x42, 0x3e, 0x91, 0xe0, 0x40, 0x04,
    0x1e, 0x02, 0x0e, 0x0c, 0xf0, 0x44, 0xfe, 0x90, 0x20, 0xfa, 0x0d, 0x20, 0xf7, 0x1d, 0x20, 0xf2,
    0x0e, 0x13, 0x24, 0x7c, 0x1e, 0x83, 0xfe, 0x62, 0x28, 0x06, 0x1e, 0xc1, 0xfe, 0x64, 0x20, 0x06,
    0x7b, 0xe2, 0x0c, 0x3e, 0x87, 0xe2, 0xf0, 0x42, 0x90, 0xe0, 0x42, 0x15, 0x20, 0xd2, 0x05, 0x20,
    0x4f, 0x16, 0x20, 0x18, 0xcb, 0x4f, 0x06, 0x04, 0xc5, 0xcb, 0x11, 0x17, 0xc1, 0xcb, 0x11, 0x17,
    0x05, 0x20, 0xf5, 0x22, 0x23, 0x22, 0x23, 0xc9, 0xce, 0xed, 0x66, 0x66, 0xcc, 0x0d, 0x00, 0x0b,
    0x03, 0x73, 0x00, 0x83, 0x00, 0x0c, 0x00, 0x0d, 0x00, 0x08, 0x11, 0x1f, 0x88, 0x89, 0x00, 0x0e,
    0xdc, 0xcc, 0x6e, 0xe6, 0xdd, 0xdd, 0xd9, 0x99, 0xbb, 0xbb, 0x67, 0x63, 0x6e, 0x0e, 0xec, 0xcc,
    0xdd, 0xdc, 0x99, 0x9f, 0xbb, 0xb9, 0x33, 0x3e, 0x3c, 0x42, 0xb9, 0xa5, 0xb9, 0xa5, 0x42, 0x3c,
    0x21, 0x04, 0x01, 0x11, 0xa8, 0x00, 0x1a, 0x13, 0xbe, 0x20, 0xfe, 0x23, 0x7d, 0xfe, 0x34, 0x20,
    0xf5, 0x06, 0x19, 0x78, 0x86, 0x23, 0x05, 0x20, 0xfb, 0x86, 0x20, 0xfe, 0x3e, 0x01, 0xe0, 0x50
};

static void mmu_dma_transfer(struct mmu *mmu) {
    uint16_t addr = (uint16_t)(mmu->gpu->reg_dma << 8);
    for(uint16_t i = 0; i < 0xA0; i++) {
        mmu->gpu->oam[i] = mmu_rb(mmu, addr + i);
    }
}


/*** Public ***/

struct mmu *mmu_new(struct cpu *cpu, struct gpu *gpu) {
    struct mmu *mmu = malloc(sizeof(struct mmu));
    mmu_init(mmu, cpu, gpu);
    return mmu;
}

void mmu_init(struct mmu *mmu, struct cpu *cpu, struct gpu *gpu) {
    memset(mmu, 0, sizeof(struct mmu));
    mmu->cpu = cpu;
    mmu->gpu = gpu;
}

void mmu_free(struct mmu *mmu) {
    free(mmu);
}

uint8_t mmu_rb(const struct mmu *mmu, const uint16_t addr) {
    switch(addr & 0xF000) {
        case 0x0000:
            /* Possibly boot ROM. */
            if(addr < 0x100 && mmu->reg_boot == 0) {
                return boot[addr];
            }
        case 0x1000:
        case 0x2000:
        case 0x3000:
            /* ROM #0 */
            return mmu->rom0[addr & 0x3FFF];
        case 0x4000:
        case 0x5000:
        case 0x6000:
        case 0x7000:
            /* ROM #1 */
            return mmu->rom1[addr & 0x3FFF];
        case 0x8000:
        case 0x9000:
            /* VRAM */
            return mmu->gpu->vram[addr & 0x1FFF];
        case 0xA000:
        case 0xB000:
            /* RAM #1 */
            return mmu->ram1[addr & 0x1FFF];
        case 0xC000:
        case 0xD000:
            /* RAM #0 */
            return mmu->ram0[addr & 0x1FFF];
        case 0xE000:
            /* RAM #0 shadow */
            return mmu->ram0[addr & 0x1FFF];
        case 0xF000:
            if(addr >= 0xF000 && addr < 0xFE00) {
                /* RAM #0 shadow */
                return mmu->ram0[addr & 0x1FFF];
            } else if(addr >= 0xFE00 && addr < 0xFEA0) {
                /* OAM */
                return mmu->gpu->oam[addr & 0xFF];
            } else if(addr >= 0xFEA0 && addr < 0xFF00) {
                /* Unusable */
                return 0;
            } else if(addr >= 0xFF00 && addr < 0xFF80) {
                /* I/O ports */
                switch(addr) {
                    case 0xFF0F: return mmu->cpu->reg_if;
                    case 0xFF40: return mmu->gpu->reg_lcdc;
                    case 0xFF41: return mmu->gpu->reg_stat;
                    case 0xFF42: return mmu->gpu->reg_scy;
                    case 0xFF43: return mmu->gpu->reg_scx;
                    case 0xFF44: return mmu->gpu->reg_ly;
                    case 0xFF45: return mmu->gpu->reg_lyc;
                    case 0xFF46: return mmu->gpu->reg_dma;
                    case 0xFF47: return mmu->gpu->reg_bgp;
                    case 0xFF48: return mmu->gpu->reg_obp0;
                    case 0xFF49: return mmu->gpu->reg_obp1;
                    case 0xFF4A: return mmu->gpu->reg_wy;
                    case 0xFF4B: return mmu->gpu->reg_wx;
                    case 0xFF50: return mmu->reg_boot;
                }
                return mmu->ports[addr & 0x7F];
            } else if(addr >= 0xFF80 && addr < 0xFFFF) {
                /* Zero RAM */
                return mmu->zram[addr & 0x7F];
            } else if(addr == 0xFFFF) {
                /* IE register. */
                return mmu->cpu->reg_ie;
            }
    }
    return 0;
}

void mmu_wb(struct mmu *mmu, const uint16_t addr, const uint8_t b) {
    switch(addr & 0xF000) {
        case 0x0000:
            /* Possibly boot ROM. */
            if(addr < 0x100 && mmu->reg_boot) {
                return;
            }
        case 0x1000:
        case 0x2000:
        case 0x3000:
            /* ROM #0 */
            mmu->rom0[addr & 0x3FFF] = b;
            return;
        case 0x4000:
        case 0x5000:
        case 0x6000:
        case 0x7000:
            /* ROM #1 */
            mmu->rom1[addr & 0x3FFF] = b;
            return;
        case 0x8000:
        case 0x9000:
            /* VRAM */
            mmu->gpu->vram[addr & 0x1FFF] = b;
            return;
        case 0xA000:
        case 0xB000:
            /* RAM #1 */
            mmu->ram1[addr & 0x1FFF] = b;
            return;
        case 0xC000:
        case 0xD000:
            /* RAM #0 */
            mmu->ram0[addr & 0x1FFF] = b;
            return;
        case 0xE000:
            /* RAM #0 shadow */
            mmu->ram0[addr & 0x1FFF] = b;
            return;
        case 0xF000:
            if(addr >= 0xF000 && addr < 0xFE00) {
                /* RAM #0 shadow */
                mmu->ram0[addr & 0x1FFF] = b;
                return;
            } else if(addr >= 0xFE00 && addr < 0xFEA0) {
                /* OAM */
                mmu->gpu->oam[addr & 0xFF] = b;
                return;
            } else if(addr >= 0xFEA0 && addr < 0xFF00) {
                /* Unusable */
                return;
            } else if(addr >= 0xFF00 && addr < 0xFF80) {
                /* I/O ports */
                switch(addr) {
                    case 0xFF0F: mmu->cpu->reg_if = b; return;
                    case 0xFF40: mmu->gpu->reg_lcdc = b; return;
                    case 0xFF41: mmu->gpu->reg_stat = b; return;
                    case 0xFF42: mmu->gpu->reg_scy = b; return;
                    case 0xFF43: mmu->gpu->reg_scx = b; return;
                    case 0xFF44: mmu->gpu->reg_ly = b; return;
                    case 0xFF45: mmu->gpu->reg_lyc = b; return;
                    case 0xFF46:
                        mmu->gpu->reg_dma = b;
                        mmu_dma_transfer(mmu);
                        return;
                    case 0xFF47: mmu->gpu->reg_bgp = b; return;
                    case 0xFF48: mmu->gpu->reg_obp0 = b; return;
                    case 0xFF49: mmu->gpu->reg_obp1 = b; return;
                    case 0xFF4A: mmu->gpu->reg_wy = b; return;
                    case 0xFF4B: mmu->gpu->reg_wx = b; return;
                    case 0xFF50: mmu->reg_boot = b; return;
                }
                mmu->ports[addr & 0x7F] = b;
                return;
            } else if(addr >= 0xFF80 && addr < 0xFFFF) {
                /* Zero RAM */
                mmu->zram[addr & 0x7F] = b;
                return;
            } else if(addr == 0xFFFF) {
                /* IE register. */
                mmu->cpu->reg_ie = b;
                return;
            }
    }
}

uint16_t mmu_rw(const struct mmu *mmu, const uint16_t addr) {
    return (uint16_t)(mmu_rb(mmu, addr) | mmu_rb(mmu, addr + 1) << 8);
}

void mmu_ww(struct mmu *mmu, const uint16_t addr, const uint16_t w) {
    mmu_wb(mmu, addr, (uint8_t)w);
    mmu_wb(mmu, (addr + 1), (uint8_t)(w >> 8));
}
