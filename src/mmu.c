#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "mmu.h"
#include "cpu.h"
#include "interrupt.h"
#include "gpu.h"
#include "timer.h"
#include "input.h"
#include "apu.h"

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

void mmu_init(struct mmu *mmu, struct cpu *cpu, struct interrupt_controller *ic, struct gpu *gpu,
        struct timer *timer, struct input *input, struct apu *apu) {
    memset(mmu, 0, sizeof(struct mmu));
    mmu->cpu = cpu;
    mmu->ic = ic;
    mmu->gpu = gpu;
    mmu->timer = timer;
    mmu->input = input;
    mmu->apu = apu;
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
            case 0xFF00: return input_io_p1(mmu->input);

            case 0xFF04: return timer_io_div(mmu->timer);
            case 0xFF05: return timer_io_tima(mmu->timer);
            case 0xFF06: return timer_io_tma(mmu->timer);
            case 0xFF07: return timer_io_tac(mmu->timer);

            case 0xFF0F: return interrupt_controller_io_if(mmu->ic);

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
                return apu_rb(mmu->apu, addr);

            case 0xFF30: case 0xFF31: case 0xFF32: case 0xFF33:
            case 0xFF34: case 0xFF35: case 0xFF36: case 0xFF37:
            case 0xFF38: case 0xFF39: case 0xFF3A: case 0xFF3B:
            case 0xFF3C: case 0xFF3D: case 0xFF3E: case 0xFF3F:
                return mmu->apu->wave_ram[addr & 0x0F];

            case 0xFF40: return gpu_io_lcdc(mmu->gpu);
            case 0xFF41: return gpu_io_stat(mmu->gpu);
            case 0xFF42: return gpu_io_scy(mmu->gpu);
            case 0xFF43: return gpu_io_scx(mmu->gpu);
            case 0xFF44: return gpu_io_ly(mmu->gpu);
            case 0xFF45: return gpu_io_lyc(mmu->gpu);
            case 0xFF46: return gpu_io_dma(mmu->gpu);
            case 0xFF47: return gpu_io_bgp(mmu->gpu);
            case 0xFF48: return gpu_io_obp0(mmu->gpu);
            case 0xFF49: return gpu_io_obp1(mmu->gpu);
            case 0xFF4A: return gpu_io_wy(mmu->gpu);
            case 0xFF4B: return gpu_io_wx(mmu->gpu);

            case 0xFF50: return mmu->reg_boot;
            default:
                return mmu->ports[addr & 0x7F] & 0x01;
        }
    } else if(addr >= 0xFF80 && addr < 0xFFFF) {
        return mmu->zram[addr & 0x7F];
    } else if(addr == 0xFFFF) {
        return interrupt_controller_io_ie(mmu->ic);
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
            case 0xFF00: input_io_set_p1(mmu->input, b); break;

            case 0xFF04: timer_io_set_div(mmu->timer, b); break;
            case 0xFF05: timer_io_set_tima(mmu->timer, b); break;
            case 0xFF06: timer_io_set_tma(mmu->timer, b); break;
            case 0xFF07: timer_io_set_tac(mmu->timer, b); break;

            case 0xFF0F: interrupt_controller_io_set_if(mmu->ic, b); break;

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
                apu_wb(mmu->apu, addr, b);
                break;

            case 0xFF30: case 0xFF31: case 0xFF32: case 0xFF33:
            case 0xFF34: case 0xFF35: case 0xFF36: case 0xFF37:
            case 0xFF38: case 0xFF39: case 0xFF3A: case 0xFF3B:
            case 0xFF3C: case 0xFF3D: case 0xFF3E: case 0xFF3F:
                mmu->apu->wave_ram[addr & 0x0F] = b;
                break;

            case 0xFF40: gpu_io_set_lcdc(mmu->gpu, b); break;
            case 0xFF41: gpu_io_set_stat(mmu->gpu, b); break;
            case 0xFF42: gpu_io_set_scy(mmu->gpu, b); break;
            case 0xFF43: gpu_io_set_scx(mmu->gpu, b); break;
            case 0xFF44: gpu_io_set_ly(mmu->gpu, b); break;
            case 0xFF45: gpu_io_set_lyc(mmu->gpu, b); break;
            case 0xFF46: gpu_io_set_dma(mmu->gpu, b); mmu_dma_transfer(mmu); break;
            case 0xFF47: gpu_io_set_bgp(mmu->gpu, b); break;
            case 0xFF48: gpu_io_set_obp0(mmu->gpu, b); break;
            case 0xFF49: gpu_io_set_obp1(mmu->gpu, b); break;
            case 0xFF4A: gpu_io_set_wy(mmu->gpu, b); break;
            case 0xFF4B: gpu_io_set_wx(mmu->gpu, b); break;

            case 0xFF50: mmu->reg_boot = 1; break; /* Can only go from 0 to 1. */
            default:
                mmu->ports[addr & 0x7F] = b;
                break;
        }
    } else if(addr >= 0xFF80 && addr < 0xFFFF) {
        mmu->zram[addr & 0x7F] = b;
    } else if(addr == 0xFFFF) {
        interrupt_controller_io_set_ie(mmu->ic, b);
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
