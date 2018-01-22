#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "mmu.h"
#include "cpu.h"
#include "interrupt.h"
#include "gpu.h"
#include "timer.h"
#include "input.h"
#include "audio.h"

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

void mmu_init(struct mmu *mmu, struct cpu *cpu, struct interrupts *interrupts, struct gpu *gpu,
        struct timer *timer, struct input *input, struct audio *audio) {
    memset(mmu, 0, sizeof(struct mmu));
    mmu->cpu = cpu;
    mmu->interrupts = interrupts;
    mmu->gpu = gpu;
    mmu->timer = timer;
    mmu->input = input;
    mmu->audio = audio;
}

void mmu_cleanup(struct mmu *mmu) {
    (void)mmu;
}

uint8_t mmu_rb(const struct mmu *mmu, const uint16_t addr) {
    if(addr < 0x100 && mmu->reg_boot == 0) {
        return boot[addr];
    } else if(addr >= 0x0000 && addr < 0x4000) {
        return mmu->rom0[addr & 0x3FFF];
    } else if(addr >= 0x4000 && addr < 0x8000) {
        return mmu->rom1[addr & 0x3FFF];
    } else if(addr >= 0x8000 && addr < 0xA000) {
        return mmu->gpu->vram[addr & 0x1FFF];
    } else if(addr >= 0xA000 && addr < 0xC000) {
        return mmu->ram1[addr & 0x1FFF];
    } else if(addr >= 0xC000 && addr < 0xE000) {
        return mmu->ram0[addr & 0x1FFF];
    } else if(addr >= 0xE000 && addr < 0xFE00) {
        return mmu->ram0[addr & 0x1FFF];
    } else if(addr >= 0xFE00 && addr < 0xFEA0) {
        return mmu->gpu->oam[addr & 0xFF];
    } else if(addr >= 0xFEA0 && addr < 0xFF00) {
        return 0;
    } else if(addr >= 0xFF00 && addr < 0xFF80) {
        switch(addr) {
            case 0xFF00: return input_rb(mmu->input);

            case 0xFF04: return mmu->timer->reg_div;
            case 0xFF05: return mmu->timer->reg_tima;
            case 0xFF06: return mmu->timer->reg_tma;
            case 0xFF07: return mmu->timer->reg_tac;

            case 0xFF0F: return mmu->interrupts->triggered;

            case 0xFF10:
            case 0xFF11:
            case 0xFF12:
            case 0xFF13:
            case 0xFF14:
            case 0xFF16:
            case 0xFF17:
            case 0xFF18:
            case 0xFF19:
            case 0xFF1A:
            case 0xFF1B:
            case 0xFF1C:
            case 0xFF1D:
            case 0xFF1E:
            case 0xFF20:
            case 0xFF21:
            case 0xFF22:
            case 0xFF23:
            case 0xFF24:
            case 0xFF25:
            case 0xFF26:
                return audio_rb(mmu->audio, addr);

            case 0xFF30: case 0xFF31: case 0xFF32: case 0xFF33:
            case 0xFF34: case 0xFF35: case 0xFF36: case 0xFF37:
            case 0xFF38: case 0xFF39: case 0xFF3A: case 0xFF3B:
            case 0xFF3C: case 0xFF3D: case 0xFF3E: case 0xFF3F:
                return mmu->audio->wave_ram[addr & 0x0F];

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
            default:
                return mmu->ports[addr & 0x7F];
        }
    } else if(addr >= 0xFF80 && addr < 0xFFFF) {
        return mmu->zram[addr & 0x7F];
    } else if(addr == 0xFFFF) {
        return mmu->interrupts->enabled;
    } else {
        return 0;
    }
}

void mmu_wb(struct mmu *mmu, const uint16_t addr, const uint8_t b) {
    if(addr < 0x100 && mmu->reg_boot == 0) {
        /* boot ROM */
        fprintf(stderr, "write to boot rom!\n");
        exit(1);
    } else if(addr >= 0x0000 && addr < 0x4000) {
        mmu->rom0[addr & 0x3FFF] = b;
    } else if(addr >= 0x4000 && addr < 0x8000) {
        mmu->rom1[addr & 0x3FFF] = b;
    } else if(addr >= 0x8000 && addr < 0xA000) {
        mmu->gpu->vram[addr & 0x1FFF] = b;
    } else if(addr >= 0xA000 && addr < 0xC000) {
        mmu->ram1[addr & 0x1FFF] = b;
    } else if(addr >= 0xC000 && addr < 0xE000) {
        mmu->ram0[addr & 0x1FFF] = b;
    } else if(addr >= 0xE000 && addr < 0xFE00) {
        mmu->ram0[addr & 0x1FFF] = b;
    } else if(addr >= 0xFE00 && addr < 0xFEA0) {
        mmu->gpu->oam[addr & 0xFF] = b;
    } else if(addr >= 0xFEA0 && addr < 0xFF00) {
        /* unusable */
    } else if(addr >= 0xFF00 && addr < 0xFF80) {
        switch(addr) {
            case 0xFF00: input_wb(mmu->input, b); break;
            case 0xFF04: mmu->timer->reg_div = 0; break;
            case 0xFF05: mmu->timer->reg_tima = b; break;
            case 0xFF06: mmu->timer->reg_tma = b; break;
            case 0xFF07: mmu->timer->reg_tac = b; break;
            case 0xFF0F: mmu->interrupts->triggered = b; break;

            case 0xFF11:
            case 0xFF12:
            case 0xFF13:
            case 0xFF14:
            case 0xFF16:
            case 0xFF17:
            case 0xFF18:
            case 0xFF19:
            case 0xFF1A:
            case 0xFF1B:
            case 0xFF1C:
            case 0xFF1D:
            case 0xFF1E:
            case 0xFF20:
            case 0xFF21:
            case 0xFF22:
            case 0xFF23:
            case 0xFF24:
            case 0xFF25:
            case 0xFF26:
                audio_wb(mmu->audio, addr, b);
                break;

            case 0xFF30: case 0xFF31: case 0xFF32: case 0xFF33:
            case 0xFF34: case 0xFF35: case 0xFF36: case 0xFF37:
            case 0xFF38: case 0xFF39: case 0xFF3A: case 0xFF3B:
            case 0xFF3C: case 0xFF3D: case 0xFF3E: case 0xFF3F:
                mmu->audio->wave_ram[addr & 0x0F] = b;
                break;

            case 0xFF40: mmu->gpu->reg_lcdc = b; break;
            case 0xFF41: mmu->gpu->reg_stat = (b & 0xF8); break;
            case 0xFF42: mmu->gpu->reg_scy = b; break;
            case 0xFF43: mmu->gpu->reg_scx = b; break;
            case 0xFF44: mmu->gpu->reg_ly = 0; break;
            case 0xFF45: mmu->gpu->reg_lyc = b; break;
            case 0xFF46: mmu->gpu->reg_dma = b; mmu_dma_transfer(mmu); break;
            case 0xFF47: mmu->gpu->reg_bgp = b; break;
            case 0xFF48: mmu->gpu->reg_obp0 = b; break;
            case 0xFF49: mmu->gpu->reg_obp1 = b; break;
            case 0xFF4A: mmu->gpu->reg_wy = b; break;
            case 0xFF4B: mmu->gpu->reg_wx = b; break;
            case 0xFF50: mmu->reg_boot = b; break;
            default:
                mmu->ports[addr & 0x7F] = b;
                break;
        }
    } else if(addr >= 0xFF80 && addr < 0xFFFF) {
        mmu->zram[addr & 0x7F] = b;
    } else if(addr == 0xFFFF) {
        mmu->interrupts->enabled = b;
    }
}

uint16_t mmu_rw(const struct mmu *mmu, const uint16_t addr) {
    return (uint16_t)(mmu_rb(mmu, addr) | mmu_rb(mmu, addr + 1) << 8);
}

void mmu_ww(struct mmu *mmu, const uint16_t addr, const uint16_t w) {
    mmu_wb(mmu, addr, (uint8_t)w);
    mmu_wb(mmu, (addr + 1), (uint8_t)(w >> 8));
}

int mmu_load_rom(struct mmu *mmu, const char *path) {
    uint8_t buf[0x8000];
    FILE *fp = fopen(path, "rb");
    if(fp == NULL) {
        return -1;
    }
    if(fread(buf, sizeof(buf), 1, fp) != 1) {
        fclose(fp);
        return -1;
    }
    fclose(fp);

    for(uint16_t i = 0; i < 0x4000; i++) {
        mmu->rom0[i] = buf[i];
        mmu->rom1[i] = buf[0x4000 + i];
    }

    return 0;
}
