#include <stdio.h>
#include <string.h>

#include "cpu.h"
#include "mmu.h"
#include "alu.h"

/*** Private ***/

static uint8_t cpu_fb(struct cpu *cpu) {
    return mmu_rb(cpu->mmu, cpu->pc++);
}

static uint16_t cpu_fw(struct cpu *cpu) {
    uint16_t w = mmu_rw(cpu->mmu, cpu->pc);
    cpu->pc += 2;
    return w;
}

static void cpu_push(struct cpu *cpu, uint16_t w) {
    cpu->sp -= 2;
    mmu_ww(cpu->mmu, cpu->sp, w);
}

static uint16_t cpu_pop(struct cpu *cpu) {
    uint16_t w = mmu_rw(cpu->mmu, cpu->sp);
    cpu->sp += 2;
    return w;
}

/*** Public ***/

void cpu_init(struct cpu *cpu, struct mmu *mmu) {
    memset(cpu, 0, sizeof(struct cpu));
    cpu->mmu = mmu;
}

void cpu_free(struct cpu *cpu) {
    (void)cpu;
}

int cpu_step(struct cpu *cpu) {
    uint8_t opcode = cpu_fb(cpu);

    switch(opcode) {

        /* 8-Bit Move Instructions */

        case 0x40: cpu->b = cpu->b; return 1;
        case 0x41: cpu->b = cpu->c; return 1;
        case 0x42: cpu->b = cpu->d; return 1;
        case 0x43: cpu->b = cpu->e; return 1;
        case 0x44: cpu->b = cpu->h; return 1;
        case 0x45: cpu->b = cpu->l; return 1;
        case 0x46: cpu->b = mmu_rb(cpu->mmu, cpu->hl); return 2;
        case 0x47: cpu->b = cpu->a; return 1;

        case 0x48: cpu->c = cpu->b; return 1;
        case 0x49: cpu->c = cpu->c; return 1;
        case 0x4A: cpu->c = cpu->d; return 1;
        case 0x4B: cpu->c = cpu->e; return 1;
        case 0x4C: cpu->c = cpu->h; return 1;
        case 0x4D: cpu->c = cpu->l; return 1;
        case 0x4E: cpu->c = mmu_rb(cpu->mmu, cpu->hl); return 2;
        case 0x4F: cpu->c = cpu->a; return 1;

        case 0x50: cpu->d = cpu->b; return 1;
        case 0x51: cpu->d = cpu->c; return 1;
        case 0x52: cpu->d = cpu->d; return 1;
        case 0x53: cpu->d = cpu->e; return 1;
        case 0x54: cpu->d = cpu->h; return 1;
        case 0x55: cpu->d = cpu->l; return 1;
        case 0x56: cpu->d = mmu_rb(cpu->mmu, cpu->hl); return 2;
        case 0x57: cpu->d = cpu->a; return 1;

        case 0x58: cpu->e = cpu->b; return 1;
        case 0x59: cpu->e = cpu->c; return 1;
        case 0x5A: cpu->e = cpu->d; return 1;
        case 0x5B: cpu->e = cpu->e; return 1;
        case 0x5C: cpu->e = cpu->h; return 1;
        case 0x5D: cpu->e = cpu->l; return 1;
        case 0x5E: cpu->e = mmu_rb(cpu->mmu, cpu->hl); return 2;
        case 0x5F: cpu->e = cpu->a; return 1;

        case 0x60: cpu->h = cpu->b; return 1;
        case 0x61: cpu->h = cpu->c; return 1;
        case 0x62: cpu->h = cpu->d; return 1;
        case 0x63: cpu->h = cpu->e; return 1;
        case 0x64: cpu->h = cpu->h; return 1;
        case 0x65: cpu->h = cpu->l; return 1;
        case 0x66: cpu->h = mmu_rb(cpu->mmu, cpu->hl); return 2;
        case 0x67: cpu->h = cpu->a; return 1;

        case 0x68: cpu->l = cpu->b; return 1;
        case 0x69: cpu->l = cpu->c; return 1;
        case 0x6A: cpu->l = cpu->d; return 1;
        case 0x6B: cpu->l = cpu->e; return 1;
        case 0x6C: cpu->l = cpu->h; return 1;
        case 0x6D: cpu->l = cpu->l; return 1;
        case 0x6E: cpu->l = mmu_rb(cpu->mmu, cpu->hl); return 2;
        case 0x6F: cpu->l = cpu->a; return 1;

        case 0x70: mmu_wb(cpu->mmu, cpu->hl, cpu->b); return 2;
        case 0x71: mmu_wb(cpu->mmu, cpu->hl, cpu->c); return 2;
        case 0x72: mmu_wb(cpu->mmu, cpu->hl, cpu->d); return 2;
        case 0x73: mmu_wb(cpu->mmu, cpu->hl, cpu->e); return 2;
        case 0x74: mmu_wb(cpu->mmu, cpu->hl, cpu->h); return 2;
        case 0x75: mmu_wb(cpu->mmu, cpu->hl, cpu->l); return 2;
        case 0x77: mmu_wb(cpu->mmu, cpu->hl, cpu->a); return 2;

        case 0x78: cpu->a = cpu->b; return 1;
        case 0x79: cpu->a = cpu->c; return 1;
        case 0x7A: cpu->a = cpu->d; return 1;
        case 0x7B: cpu->a = cpu->e; return 1;
        case 0x7C: cpu->a = cpu->h; return 1;
        case 0x7D: cpu->a = cpu->l; return 1;
        case 0x7E: cpu->a = mmu_rb(cpu->mmu, cpu->hl); return 2;
        case 0x7F: cpu->a = cpu->a; return 1;

        case 0x02: mmu_wb(cpu->mmu, cpu->bc, cpu->a); return 2;
        case 0x12: mmu_wb(cpu->mmu, cpu->de, cpu->a); return 2;
        case 0x22: mmu_wb(cpu->mmu, cpu->hl++, cpu->a); return 2;
        case 0x32: mmu_wb(cpu->mmu, cpu->hl--, cpu->a); return 2;

        case 0x06: cpu->b = cpu_fb(cpu); return 2;
        case 0x0E: cpu->c = cpu_fb(cpu); return 2;
        case 0x16: cpu->d = cpu_fb(cpu); return 2;
        case 0x1E: cpu->e = cpu_fb(cpu); return 2;
        case 0x26: cpu->h = cpu_fb(cpu); return 2;
        case 0x2E: cpu->l = cpu_fb(cpu); return 2;
        case 0x36: mmu_wb(cpu->mmu, cpu->hl, cpu_fb(cpu)); return 3;
        case 0x3E: cpu->a = cpu_fb(cpu); return 2;

        case 0x0A: cpu->a = mmu_rb(cpu->mmu, cpu->bc); return 2;
        case 0x1A: cpu->a = mmu_rb(cpu->mmu, cpu->de); return 2;
        case 0x2A: cpu->a = mmu_rb(cpu->mmu, cpu->hl++); return 2;
        case 0x3A: cpu->a = mmu_rb(cpu->mmu, cpu->hl--); return 2;

        case 0xE0: mmu_wb(cpu->mmu, 0xFF00 + cpu_fb(cpu), cpu->a); return 3;
        case 0xF0: cpu->a = mmu_rb(cpu->mmu, 0xFF00 + cpu_fb(cpu)); return 3;

        case 0xE3: mmu_wb(cpu->mmu, 0xFF00 + cpu->c, cpu->a); return 2;
        case 0xF3: cpu->a = mmu_rb(cpu->mmu, 0xFF00 + cpu->c); return 2;

        /* 16-Bit Move Instructions */

        case 0x01: cpu->bc = cpu_fw(cpu); return 3;
        case 0x11: cpu->de = cpu_fw(cpu); return 3;
        case 0x21: cpu->hl = cpu_fw(cpu); return 3;
        case 0x31: cpu->sp = cpu_fw(cpu); return 3;

        case 0xC1: cpu->bc = cpu_pop(cpu); return 3;
        case 0xD1: cpu->de = cpu_pop(cpu); return 3;
        case 0xE1: cpu->hl = cpu_pop(cpu); return 3;
        case 0xF1: cpu->af = cpu_pop(cpu); return 3;

        case 0xC5: cpu_push(cpu, cpu->bc); return 4;
        case 0xD5: cpu_push(cpu, cpu->de); return 4;
        case 0xE5: cpu_push(cpu, cpu->hl); return 4;
        case 0xF5: cpu_push(cpu, cpu->af); return 4;

        case 0x08: mmu_ww(cpu->mmu, cpu_fw(cpu), cpu->sp); return 5;
        case 0xF9: cpu->sp = cpu->hl; return 2;
        case 0xF8: cpu->hl = alu_add_u16_i8(cpu, cpu->sp, (int8_t)cpu_fb(cpu)); return 3;

        /* 8-Bit ALU Instructions */

        case 0x80: cpu->a = alu_add(cpu, cpu->a, cpu->b); return 1;
        case 0x81: cpu->a = alu_add(cpu, cpu->a, cpu->c); return 1;
        case 0x82: cpu->a = alu_add(cpu, cpu->a, cpu->d); return 1;
        case 0x83: cpu->a = alu_add(cpu, cpu->a, cpu->e); return 1;
        case 0x84: cpu->a = alu_add(cpu, cpu->a, cpu->h); return 1;
        case 0x85: cpu->a = alu_add(cpu, cpu->a, cpu->l); return 1;
        case 0x86: cpu->a = alu_add(cpu, cpu->a, mmu_rb(cpu->mmu, cpu->hl)); return 2;
        case 0x87: cpu->a = alu_add(cpu, cpu->a, cpu->a); return 1;

        case 0x88: cpu->a = alu_adc(cpu, cpu->a, cpu->b); return 1;
        case 0x89: cpu->a = alu_adc(cpu, cpu->a, cpu->c); return 1;
        case 0x8A: cpu->a = alu_adc(cpu, cpu->a, cpu->d); return 1;
        case 0x8B: cpu->a = alu_adc(cpu, cpu->a, cpu->e); return 1;
        case 0x8C: cpu->a = alu_adc(cpu, cpu->a, cpu->h); return 1;
        case 0x8D: cpu->a = alu_adc(cpu, cpu->a, cpu->l); return 1;
        case 0x8E: cpu->a = alu_adc(cpu, cpu->a, mmu_rb(cpu->mmu, cpu->hl)); return 2;
        case 0x8F: cpu->a = alu_adc(cpu, cpu->a, cpu->a); return 1;

        case 0x90: cpu->a = alu_sub(cpu, cpu->a, cpu->b); return 1;
        case 0x91: cpu->a = alu_sub(cpu, cpu->a, cpu->c); return 1;
        case 0x92: cpu->a = alu_sub(cpu, cpu->a, cpu->d); return 1;
        case 0x93: cpu->a = alu_sub(cpu, cpu->a, cpu->e); return 1;
        case 0x94: cpu->a = alu_sub(cpu, cpu->a, cpu->h); return 1;
        case 0x95: cpu->a = alu_sub(cpu, cpu->a, cpu->l); return 1;
        case 0x96: cpu->a = alu_sub(cpu, cpu->a, mmu_rb(cpu->mmu, cpu->hl)); return 2;
        case 0x97: cpu->a = alu_sub(cpu, cpu->a, cpu->a); return 1;

        case 0x98: cpu->a = alu_sbc(cpu, cpu->a, cpu->b); return 1;
        case 0x99: cpu->a = alu_sbc(cpu, cpu->a, cpu->c); return 1;
        case 0x9A: cpu->a = alu_sbc(cpu, cpu->a, cpu->d); return 1;
        case 0x9B: cpu->a = alu_sbc(cpu, cpu->a, cpu->e); return 1;
        case 0x9C: cpu->a = alu_sbc(cpu, cpu->a, cpu->h); return 1;
        case 0x9D: cpu->a = alu_sbc(cpu, cpu->a, cpu->l); return 1;
        case 0x9E: cpu->a = alu_sbc(cpu, cpu->a, mmu_rb(cpu->mmu, cpu->hl)); return 2;
        case 0x9F: cpu->a = alu_sbc(cpu, cpu->a, cpu->a); return 1;

        case 0xA0: cpu->a = alu_and(cpu, cpu->a, cpu->b); return 1;
        case 0xA1: cpu->a = alu_and(cpu, cpu->a, cpu->c); return 1;
        case 0xA2: cpu->a = alu_and(cpu, cpu->a, cpu->d); return 1;
        case 0xA3: cpu->a = alu_and(cpu, cpu->a, cpu->e); return 1;
        case 0xA4: cpu->a = alu_and(cpu, cpu->a, cpu->h); return 1;
        case 0xA5: cpu->a = alu_and(cpu, cpu->a, cpu->l); return 1;
        case 0xA6: cpu->a = alu_and(cpu, cpu->a, mmu_rb(cpu->mmu, cpu->hl)); return 2;
        case 0xA7: cpu->a = alu_and(cpu, cpu->a, cpu->a); return 1;

        case 0xA8: cpu->a = alu_xor(cpu, cpu->a, cpu->b); return 1;
        case 0xA9: cpu->a = alu_xor(cpu, cpu->a, cpu->c); return 1;
        case 0xAA: cpu->a = alu_xor(cpu, cpu->a, cpu->d); return 1;
        case 0xAB: cpu->a = alu_xor(cpu, cpu->a, cpu->e); return 1;
        case 0xAC: cpu->a = alu_xor(cpu, cpu->a, cpu->h); return 1;
        case 0xAD: cpu->a = alu_xor(cpu, cpu->a, cpu->l); return 1;
        case 0xAE: cpu->a = alu_xor(cpu, cpu->a, mmu_rb(cpu->mmu, cpu->hl)); return 2;
        case 0xAF: cpu->a = alu_xor(cpu, cpu->a, cpu->a); return 1;

        case 0xB0: cpu->a = alu_or(cpu, cpu->a, cpu->b); return 1;
        case 0xB1: cpu->a = alu_or(cpu, cpu->a, cpu->c); return 1;
        case 0xB2: cpu->a = alu_or(cpu, cpu->a, cpu->d); return 1;
        case 0xB3: cpu->a = alu_or(cpu, cpu->a, cpu->e); return 1;
        case 0xB4: cpu->a = alu_or(cpu, cpu->a, cpu->h); return 1;
        case 0xB5: cpu->a = alu_or(cpu, cpu->a, cpu->l); return 1;
        case 0xB6: cpu->a = alu_or(cpu, cpu->a, mmu_rb(cpu->mmu, cpu->hl)); return 2;
        case 0xB7: cpu->a = alu_or(cpu, cpu->a, cpu->a); return 1;

        case 0xB8: alu_cp(cpu, cpu->a, cpu->b); return 1;
        case 0xB9: alu_cp(cpu, cpu->a, cpu->c); return 1;
        case 0xBA: alu_cp(cpu, cpu->a, cpu->d); return 1;
        case 0xBB: alu_cp(cpu, cpu->a, cpu->e); return 1;
        case 0xBC: alu_cp(cpu, cpu->a, cpu->h); return 1;
        case 0xBD: alu_cp(cpu, cpu->a, cpu->l); return 1;
        case 0xBE: alu_cp(cpu, cpu->a, mmu_rb(cpu->mmu, cpu->hl)); return 2;
        case 0xBF: alu_cp(cpu, cpu->a, cpu->a); return 1;

        case 0xC6: cpu->a = alu_add(cpu, cpu->a, cpu_fb(cpu)); return 2;
        case 0xCE: cpu->a = alu_adc(cpu, cpu->a, cpu_fb(cpu)); return 2;
        case 0xD6: cpu->a = alu_sub(cpu, cpu->a, cpu_fb(cpu)); return 2;
        case 0xDE: cpu->a = alu_sbc(cpu, cpu->a, cpu_fb(cpu)); return 2;
        case 0xE6: cpu->a = alu_and(cpu, cpu->a, cpu_fb(cpu)); return 2;
        case 0xEE: cpu->a = alu_xor(cpu, cpu->a, cpu_fb(cpu)); return 2;
        case 0xF6: cpu->a = alu_or(cpu, cpu->a, cpu_fb(cpu)); return 2;
        case 0xFE: alu_cp(cpu, cpu->a, cpu_fb(cpu)); return 2;

    }

    fprintf(stderr, "unimplemented opcode: 0x%02X\n", opcode);
    return 0;
}

void cpu_run(struct cpu *cpu) {
    int cycles;

    while(1) {
        cycles = cpu_step(cpu);
        if(cycles == 0) {
            fprintf(stderr, "cpu_step returned 0 cycles\n");
            break;
        }
    }
}

uint8_t cpu_flag(struct cpu *cpu, uint8_t flag) {
    return ((cpu->f & flag) == flag);
}

void cpu_set_flag(struct cpu *cpu, uint8_t flag, int cond) {
    if(cond) {
        cpu->f |= flag;
    } else {
        cpu->f &= ~flag;
    }
}
