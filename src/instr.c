#include <string.h>
#include "instr.h"
#include "instr_info.h"
#include "alu.h"
#include "cpu.h"
#include "mmu.h"

/*** Private ***/

#if 0
static void instr_debug(struct cpu *cpu, const struct instr_info *info) {
    uint16_t pc = cpu->pc - 1;
    printf("%04X: ", pc);

    for(uint16_t i = 0; i < 8; i++) {
        if(i < info->size) {
            printf("%02X ", mmu_rb(cpu->mmu, pc + i));
        } else {
            printf("   ");
        }
    }

    printf("%s ", info->desc);

    for(size_t i = 0; i < 20 - strlen(info->desc); i++) {
        printf(" ");
    }

    if(info->size == 2) {
        printf(" (0x%02X)", mmu_rb(cpu->mmu, pc + 1));
    } else if(info->size == 3) {
        printf(" (0x%04X)", mmu_rw(cpu->mmu, pc + 1));
    }

    if(strcmp(info->desc, "CP d8") == 0) {
        printf(" (A=0x%02X)", cpu->a);
    }

    printf("\n");
}
#define DEBUG_INSTR() instr_debug(cpu, info)
#else
#define DEBUG_INSTR()
#endif

static int cpu_jr_if(struct cpu *cpu, const struct instr_info *info, int cond) {
    int8_t a = (int8_t)cpu_fb(cpu);
    if(cond) {
        cpu->pc = (uint16_t)(cpu->pc + a);
        return info->cond_cycles;
    }
    return info->cycles;
}

static int cpu_jp_if(struct cpu *cpu, const struct instr_info *info, int cond) {
    uint16_t addr = cpu_fw(cpu);
    if(cond) {
        cpu->pc = addr;
        return info->cond_cycles;
    }
    return info->cycles;
}

static int cpu_call_if(struct cpu *cpu, const struct instr_info *info, int cond) {
    uint16_t addr = cpu_fw(cpu);
    if(cond) {
        cpu_push(cpu, cpu->pc);
        cpu->pc = addr;
        return info->cycles;
    }
    return info->cycles;
}

static int cpu_ret_if(struct cpu *cpu, const struct instr_info *info, int cond) {
    if(cond) {
        cpu->pc = cpu_pop(cpu);
        return info->cycles;
    }
    return info->cycles;
}

static int instr_prefix(struct cpu *cpu) {
    uint8_t opcode = cpu_fb(cpu);
    const struct instr_info *info = &INSTR_INFOS_PREFIX[opcode];
    uint8_t op = (opcode >> 5) & 0x03;
    uint8_t n = (opcode >> 3) & 0x07;
    uint8_t r = (opcode >> 0) & 0x07;
    uint8_t x, new_x;

    DEBUG_INSTR();

    switch(r) {
        case 0: x = cpu->b; break;
        case 1: x = cpu->c; break;
        case 2: x = cpu->d; break;
        case 3: x = cpu->e; break;
        case 4: x = cpu->h; break;
        case 5: x = cpu->l; break;
        case 6: x = mmu_rb(cpu->mmu, cpu->hl); break;
        case 7: x = cpu->a; break;
        default: return 0;
    }

    new_x = x;

    switch(op) {
        case 0:
            switch(n) {
                case 0: new_x = alu_rlc(cpu, x); break;
                case 1: new_x = alu_rrc(cpu, x); break;
                case 2: new_x = alu_rl(cpu, x); break;
                case 3: new_x = alu_rr(cpu, x); break;
                case 4: new_x = alu_sla(cpu, x); break;
                case 5: new_x = alu_sra(cpu, x); break;
                case 6: new_x = alu_swap(cpu, x); break;
                case 7: new_x = alu_srl(cpu, x); break;
            }
            break;
        case 1: alu_bit(cpu, x, n); break;
        case 2: new_x = alu_res(cpu, x, n); break;
        case 3: new_x = alu_set(cpu, x, n); break;
    }

    if(new_x != x) {
        switch(r) {
            case 0: cpu->b = new_x; break;
            case 1: cpu->c = new_x; break;
            case 2: cpu->d = new_x; break;
            case 3: cpu->e = new_x; break;
            case 4: cpu->h = new_x; break;
            case 5: cpu->l = new_x; break;
            case 6: mmu_wb(cpu->mmu, cpu->hl, new_x); break;
            case 7: cpu->a = new_x; break;
        }
    }

    return info->cycles;
}

/*** Public ***/

int instr_fetch_decode_execute(struct cpu *cpu) {
    uint8_t opcode = cpu_fb(cpu);
    const struct instr_info *info = &INSTR_INFOS[opcode];
    DEBUG_INSTR();

    switch(opcode) {

        /* CPU Control Instructions */

        case 0x00: return info->cycles;
        case 0x10: cpu->stop = 1; return info->cycles;
        case 0xF3: cpu->ime = 0; return info->cycles;
        case 0x76: cpu->halt = 1; return info->cycles;
        case 0xFB: cpu->ime = 1; return info->cycles;
        case 0xCB: return instr_prefix(cpu);

        /* Jump and Call Instructions */

        case 0x18: return cpu_jr_if(cpu, info, 1);
        case 0x20: return cpu_jr_if(cpu, info, !cpu_flag(cpu, FLAG_Z));
        case 0x30: return cpu_jr_if(cpu, info, !cpu_flag(cpu, FLAG_C));
        case 0x28: return cpu_jr_if(cpu, info, cpu_flag(cpu, FLAG_Z));
        case 0x38: return cpu_jr_if(cpu, info, cpu_flag(cpu, FLAG_C));

        case 0xC3: return cpu_jp_if(cpu, info, 1);
        case 0xC2: return cpu_jp_if(cpu, info, !cpu_flag(cpu, FLAG_Z));
        case 0xD2: return cpu_jp_if(cpu, info, !cpu_flag(cpu, FLAG_C));
        case 0xCA: return cpu_jp_if(cpu, info, cpu_flag(cpu, FLAG_Z));
        case 0xDA: return cpu_jp_if(cpu, info, cpu_flag(cpu, FLAG_C));

        case 0xCD: return cpu_call_if(cpu, info, 1);
        case 0xC4: return cpu_call_if(cpu, info, !cpu_flag(cpu, FLAG_Z));
        case 0xD4: return cpu_call_if(cpu, info, !cpu_flag(cpu, FLAG_C));
        case 0xCC: return cpu_call_if(cpu, info, cpu_flag(cpu, FLAG_Z));
        case 0xDC: return cpu_call_if(cpu, info, cpu_flag(cpu, FLAG_C));

        case 0xC0: return cpu_ret_if(cpu, info, !cpu_flag(cpu, FLAG_Z));
        case 0xD0: return cpu_ret_if(cpu, info, !cpu_flag(cpu, FLAG_C));
        case 0xC8: return cpu_ret_if(cpu, info, cpu_flag(cpu, FLAG_Z));
        case 0xD8: return cpu_ret_if(cpu, info, cpu_flag(cpu, FLAG_C));

        case 0xC9: cpu->pc = cpu_pop(cpu); return info->cycles;
        case 0xD9: cpu->ime = 1; cpu->pc = cpu_pop(cpu); return info->cycles;

        case 0xE9: cpu->pc = mmu_rb(cpu->mmu, cpu->hl); return info->cycles;

        case 0xC7: cpu_push(cpu, cpu->pc); cpu->pc = 0x0000; return info->cycles;
        case 0xCF: cpu_push(cpu, cpu->pc); cpu->pc = 0x0008; return info->cycles;
        case 0xD7: cpu_push(cpu, cpu->pc); cpu->pc = 0x0010; return info->cycles;
        case 0xDF: cpu_push(cpu, cpu->pc); cpu->pc = 0x0018; return info->cycles;
        case 0xE7: cpu_push(cpu, cpu->pc); cpu->pc = 0x0020; return info->cycles;
        case 0xEF: cpu_push(cpu, cpu->pc); cpu->pc = 0x0028; return info->cycles;
        case 0xF7: cpu_push(cpu, cpu->pc); cpu->pc = 0x0030; return info->cycles;
        case 0xFF: cpu_push(cpu, cpu->pc); cpu->pc = 0x0038; return info->cycles;

        /* 8-Bit Move Instructions */

        case 0x40: cpu->b = cpu->b; return info->cycles;
        case 0x41: cpu->b = cpu->c; return info->cycles;
        case 0x42: cpu->b = cpu->d; return info->cycles;
        case 0x43: cpu->b = cpu->e; return info->cycles;
        case 0x44: cpu->b = cpu->h; return info->cycles;
        case 0x45: cpu->b = cpu->l; return info->cycles;
        case 0x46: cpu->b = mmu_rb(cpu->mmu, cpu->hl); return info->cycles;
        case 0x47: cpu->b = cpu->a; return info->cycles;

        case 0x48: cpu->c = cpu->b; return info->cycles;
        case 0x49: cpu->c = cpu->c; return info->cycles;
        case 0x4A: cpu->c = cpu->d; return info->cycles;
        case 0x4B: cpu->c = cpu->e; return info->cycles;
        case 0x4C: cpu->c = cpu->h; return info->cycles;
        case 0x4D: cpu->c = cpu->l; return info->cycles;
        case 0x4E: cpu->c = mmu_rb(cpu->mmu, cpu->hl); return info->cycles;
        case 0x4F: cpu->c = cpu->a; return info->cycles;

        case 0x50: cpu->d = cpu->b; return info->cycles;
        case 0x51: cpu->d = cpu->c; return info->cycles;
        case 0x52: cpu->d = cpu->d; return info->cycles;
        case 0x53: cpu->d = cpu->e; return info->cycles;
        case 0x54: cpu->d = cpu->h; return info->cycles;
        case 0x55: cpu->d = cpu->l; return info->cycles;
        case 0x56: cpu->d = mmu_rb(cpu->mmu, cpu->hl); return info->cycles;
        case 0x57: cpu->d = cpu->a; return info->cycles;

        case 0x58: cpu->e = cpu->b; return info->cycles;
        case 0x59: cpu->e = cpu->c; return info->cycles;
        case 0x5A: cpu->e = cpu->d; return info->cycles;
        case 0x5B: cpu->e = cpu->e; return info->cycles;
        case 0x5C: cpu->e = cpu->h; return info->cycles;
        case 0x5D: cpu->e = cpu->l; return info->cycles;
        case 0x5E: cpu->e = mmu_rb(cpu->mmu, cpu->hl); return info->cycles;
        case 0x5F: cpu->e = cpu->a; return info->cycles;

        case 0x60: cpu->h = cpu->b; return info->cycles;
        case 0x61: cpu->h = cpu->c; return info->cycles;
        case 0x62: cpu->h = cpu->d; return info->cycles;
        case 0x63: cpu->h = cpu->e; return info->cycles;
        case 0x64: cpu->h = cpu->h; return info->cycles;
        case 0x65: cpu->h = cpu->l; return info->cycles;
        case 0x66: cpu->h = mmu_rb(cpu->mmu, cpu->hl); return info->cycles;
        case 0x67: cpu->h = cpu->a; return info->cycles;

        case 0x68: cpu->l = cpu->b; return info->cycles;
        case 0x69: cpu->l = cpu->c; return info->cycles;
        case 0x6A: cpu->l = cpu->d; return info->cycles;
        case 0x6B: cpu->l = cpu->e; return info->cycles;
        case 0x6C: cpu->l = cpu->h; return info->cycles;
        case 0x6D: cpu->l = cpu->l; return info->cycles;
        case 0x6E: cpu->l = mmu_rb(cpu->mmu, cpu->hl); return info->cycles;
        case 0x6F: cpu->l = cpu->a; return info->cycles;

        case 0x70: mmu_wb(cpu->mmu, cpu->hl, cpu->b); return info->cycles;
        case 0x71: mmu_wb(cpu->mmu, cpu->hl, cpu->c); return info->cycles;
        case 0x72: mmu_wb(cpu->mmu, cpu->hl, cpu->d); return info->cycles;
        case 0x73: mmu_wb(cpu->mmu, cpu->hl, cpu->e); return info->cycles;
        case 0x74: mmu_wb(cpu->mmu, cpu->hl, cpu->h); return info->cycles;
        case 0x75: mmu_wb(cpu->mmu, cpu->hl, cpu->l); return info->cycles;
        case 0x77: mmu_wb(cpu->mmu, cpu->hl, cpu->a); return info->cycles;

        case 0x78: cpu->a = cpu->b; return info->cycles;
        case 0x79: cpu->a = cpu->c; return info->cycles;
        case 0x7A: cpu->a = cpu->d; return info->cycles;
        case 0x7B: cpu->a = cpu->e; return info->cycles;
        case 0x7C: cpu->a = cpu->h; return info->cycles;
        case 0x7D: cpu->a = cpu->l; return info->cycles;
        case 0x7E: cpu->a = mmu_rb(cpu->mmu, cpu->hl); return info->cycles;
        case 0x7F: cpu->a = cpu->a; return info->cycles;

        case 0x02: mmu_wb(cpu->mmu, cpu->bc, cpu->a); return info->cycles;
        case 0x12: mmu_wb(cpu->mmu, cpu->de, cpu->a); return info->cycles;
        case 0x22: mmu_wb(cpu->mmu, cpu->hl++, cpu->a); return info->cycles;
        case 0x32: mmu_wb(cpu->mmu, cpu->hl--, cpu->a); return info->cycles;

        case 0x06: cpu->b = cpu_fb(cpu); return info->cycles;
        case 0x0E: cpu->c = cpu_fb(cpu); return info->cycles;
        case 0x16: cpu->d = cpu_fb(cpu); return info->cycles;
        case 0x1E: cpu->e = cpu_fb(cpu); return info->cycles;
        case 0x26: cpu->h = cpu_fb(cpu); return info->cycles;
        case 0x2E: cpu->l = cpu_fb(cpu); return info->cycles;
        case 0x36: mmu_wb(cpu->mmu, cpu->hl, cpu_fb(cpu)); return info->cycles;
        case 0x3E: cpu->a = cpu_fb(cpu); return info->cycles;

        case 0x0A: cpu->a = mmu_rb(cpu->mmu, cpu->bc); return info->cycles;
        case 0x1A: cpu->a = mmu_rb(cpu->mmu, cpu->de); return info->cycles;
        case 0x2A: cpu->a = mmu_rb(cpu->mmu, cpu->hl++); return info->cycles;
        case 0x3A: cpu->a = mmu_rb(cpu->mmu, cpu->hl--); return info->cycles;

        case 0xE0: mmu_wb(cpu->mmu, 0xFF00 + cpu_fb(cpu), cpu->a); return info->cycles;
        case 0xF0: cpu->a = mmu_rb(cpu->mmu, 0xFF00 + cpu_fb(cpu)); return info->cycles;

        case 0xE2: mmu_wb(cpu->mmu, 0xFF00 + cpu->c, cpu->a); return info->cycles;
        case 0xF2: cpu->a = mmu_rb(cpu->mmu, 0xFF00 + cpu->c); return info->cycles;

        case 0xEA: mmu_wb(cpu->mmu, cpu_fw(cpu), cpu->a); return info->cycles;
        case 0xFA: cpu->a = mmu_rb(cpu->mmu, cpu_fw(cpu)); return info->cycles;

        /* 16-Bit Move Instructions */

        case 0x01: cpu->bc = cpu_fw(cpu); return info->cycles;
        case 0x11: cpu->de = cpu_fw(cpu); return info->cycles;
        case 0x21: cpu->hl = cpu_fw(cpu); return info->cycles;
        case 0x31: cpu->sp = cpu_fw(cpu); return info->cycles;

        case 0xC1: cpu->bc = cpu_pop(cpu); return info->cycles;
        case 0xD1: cpu->de = cpu_pop(cpu); return info->cycles;
        case 0xE1: cpu->hl = cpu_pop(cpu); return info->cycles;
        case 0xF1: cpu->af = cpu_pop(cpu); return info->cycles;

        case 0xC5: cpu_push(cpu, cpu->bc); return info->cycles;
        case 0xD5: cpu_push(cpu, cpu->de); return info->cycles;
        case 0xE5: cpu_push(cpu, cpu->hl); return info->cycles;
        case 0xF5: cpu_push(cpu, cpu->af); return info->cycles;

        case 0x08: mmu_ww(cpu->mmu, cpu_fw(cpu), cpu->sp); return info->cycles;
        case 0xF9: cpu->sp = cpu->hl; return info->cycles;
        case 0xF8: cpu->hl = alu_add_u16_i8(cpu, cpu->sp, (int8_t)cpu_fb(cpu)); return info->cycles;

        /* 8-Bit ALU Instructions */

        case 0x80: cpu->a = alu_add(cpu, cpu->a, cpu->b); return info->cycles;
        case 0x81: cpu->a = alu_add(cpu, cpu->a, cpu->c); return info->cycles;
        case 0x82: cpu->a = alu_add(cpu, cpu->a, cpu->d); return info->cycles;
        case 0x83: cpu->a = alu_add(cpu, cpu->a, cpu->e); return info->cycles;
        case 0x84: cpu->a = alu_add(cpu, cpu->a, cpu->h); return info->cycles;
        case 0x85: cpu->a = alu_add(cpu, cpu->a, cpu->l); return info->cycles;
        case 0x86: cpu->a = alu_add(cpu, cpu->a, mmu_rb(cpu->mmu, cpu->hl)); return info->cycles;
        case 0x87: cpu->a = alu_add(cpu, cpu->a, cpu->a); return info->cycles;

        case 0x88: cpu->a = alu_adc(cpu, cpu->a, cpu->b); return info->cycles;
        case 0x89: cpu->a = alu_adc(cpu, cpu->a, cpu->c); return info->cycles;
        case 0x8A: cpu->a = alu_adc(cpu, cpu->a, cpu->d); return info->cycles;
        case 0x8B: cpu->a = alu_adc(cpu, cpu->a, cpu->e); return info->cycles;
        case 0x8C: cpu->a = alu_adc(cpu, cpu->a, cpu->h); return info->cycles;
        case 0x8D: cpu->a = alu_adc(cpu, cpu->a, cpu->l); return info->cycles;
        case 0x8E: cpu->a = alu_adc(cpu, cpu->a, mmu_rb(cpu->mmu, cpu->hl)); return info->cycles;
        case 0x8F: cpu->a = alu_adc(cpu, cpu->a, cpu->a); return info->cycles;

        case 0x90: cpu->a = alu_sub(cpu, cpu->a, cpu->b); return info->cycles;
        case 0x91: cpu->a = alu_sub(cpu, cpu->a, cpu->c); return info->cycles;
        case 0x92: cpu->a = alu_sub(cpu, cpu->a, cpu->d); return info->cycles;
        case 0x93: cpu->a = alu_sub(cpu, cpu->a, cpu->e); return info->cycles;
        case 0x94: cpu->a = alu_sub(cpu, cpu->a, cpu->h); return info->cycles;
        case 0x95: cpu->a = alu_sub(cpu, cpu->a, cpu->l); return info->cycles;
        case 0x96: cpu->a = alu_sub(cpu, cpu->a, mmu_rb(cpu->mmu, cpu->hl)); return info->cycles;
        case 0x97: cpu->a = alu_sub(cpu, cpu->a, cpu->a); return info->cycles;

        case 0x98: cpu->a = alu_sbc(cpu, cpu->a, cpu->b); return info->cycles;
        case 0x99: cpu->a = alu_sbc(cpu, cpu->a, cpu->c); return info->cycles;
        case 0x9A: cpu->a = alu_sbc(cpu, cpu->a, cpu->d); return info->cycles;
        case 0x9B: cpu->a = alu_sbc(cpu, cpu->a, cpu->e); return info->cycles;
        case 0x9C: cpu->a = alu_sbc(cpu, cpu->a, cpu->h); return info->cycles;
        case 0x9D: cpu->a = alu_sbc(cpu, cpu->a, cpu->l); return info->cycles;
        case 0x9E: cpu->a = alu_sbc(cpu, cpu->a, mmu_rb(cpu->mmu, cpu->hl)); return info->cycles;
        case 0x9F: cpu->a = alu_sbc(cpu, cpu->a, cpu->a); return info->cycles;

        case 0xA0: cpu->a = alu_and(cpu, cpu->a, cpu->b); return info->cycles;
        case 0xA1: cpu->a = alu_and(cpu, cpu->a, cpu->c); return info->cycles;
        case 0xA2: cpu->a = alu_and(cpu, cpu->a, cpu->d); return info->cycles;
        case 0xA3: cpu->a = alu_and(cpu, cpu->a, cpu->e); return info->cycles;
        case 0xA4: cpu->a = alu_and(cpu, cpu->a, cpu->h); return info->cycles;
        case 0xA5: cpu->a = alu_and(cpu, cpu->a, cpu->l); return info->cycles;
        case 0xA6: cpu->a = alu_and(cpu, cpu->a, mmu_rb(cpu->mmu, cpu->hl)); return info->cycles;
        case 0xA7: cpu->a = alu_and(cpu, cpu->a, cpu->a); return info->cycles;

        case 0xA8: cpu->a = alu_xor(cpu, cpu->a, cpu->b); return info->cycles;
        case 0xA9: cpu->a = alu_xor(cpu, cpu->a, cpu->c); return info->cycles;
        case 0xAA: cpu->a = alu_xor(cpu, cpu->a, cpu->d); return info->cycles;
        case 0xAB: cpu->a = alu_xor(cpu, cpu->a, cpu->e); return info->cycles;
        case 0xAC: cpu->a = alu_xor(cpu, cpu->a, cpu->h); return info->cycles;
        case 0xAD: cpu->a = alu_xor(cpu, cpu->a, cpu->l); return info->cycles;
        case 0xAE: cpu->a = alu_xor(cpu, cpu->a, mmu_rb(cpu->mmu, cpu->hl)); return info->cycles;
        case 0xAF: cpu->a = alu_xor(cpu, cpu->a, cpu->a); return info->cycles;

        case 0xB0: cpu->a = alu_or(cpu, cpu->a, cpu->b); return info->cycles;
        case 0xB1: cpu->a = alu_or(cpu, cpu->a, cpu->c); return info->cycles;
        case 0xB2: cpu->a = alu_or(cpu, cpu->a, cpu->d); return info->cycles;
        case 0xB3: cpu->a = alu_or(cpu, cpu->a, cpu->e); return info->cycles;
        case 0xB4: cpu->a = alu_or(cpu, cpu->a, cpu->h); return info->cycles;
        case 0xB5: cpu->a = alu_or(cpu, cpu->a, cpu->l); return info->cycles;
        case 0xB6: cpu->a = alu_or(cpu, cpu->a, mmu_rb(cpu->mmu, cpu->hl)); return info->cycles;
        case 0xB7: cpu->a = alu_or(cpu, cpu->a, cpu->a); return info->cycles;

        case 0xB8: alu_cp(cpu, cpu->a, cpu->b); return info->cycles;
        case 0xB9: alu_cp(cpu, cpu->a, cpu->c); return info->cycles;
        case 0xBA: alu_cp(cpu, cpu->a, cpu->d); return info->cycles;
        case 0xBB: alu_cp(cpu, cpu->a, cpu->e); return info->cycles;
        case 0xBC: alu_cp(cpu, cpu->a, cpu->h); return info->cycles;
        case 0xBD: alu_cp(cpu, cpu->a, cpu->l); return info->cycles;
        case 0xBE: alu_cp(cpu, cpu->a, mmu_rb(cpu->mmu, cpu->hl)); return info->cycles;
        case 0xBF: alu_cp(cpu, cpu->a, cpu->a); return info->cycles;

        case 0xC6: cpu->a = alu_add(cpu, cpu->a, cpu_fb(cpu)); return info->cycles;
        case 0xCE: cpu->a = alu_adc(cpu, cpu->a, cpu_fb(cpu)); return info->cycles;
        case 0xD6: cpu->a = alu_sub(cpu, cpu->a, cpu_fb(cpu)); return info->cycles;
        case 0xDE: cpu->a = alu_sbc(cpu, cpu->a, cpu_fb(cpu)); return info->cycles;
        case 0xE6: cpu->a = alu_and(cpu, cpu->a, cpu_fb(cpu)); return info->cycles;
        case 0xEE: cpu->a = alu_xor(cpu, cpu->a, cpu_fb(cpu)); return info->cycles;
        case 0xF6: cpu->a = alu_or(cpu, cpu->a, cpu_fb(cpu)); return info->cycles;
        case 0xFE: alu_cp(cpu, cpu->a, cpu_fb(cpu)); return info->cycles;

        case 0x04: cpu->b = alu_inc(cpu, cpu->b); return info->cycles;
        case 0x0C: cpu->c = alu_inc(cpu, cpu->c); return info->cycles;
        case 0x14: cpu->d = alu_inc(cpu, cpu->d); return info->cycles;
        case 0x1C: cpu->e = alu_inc(cpu, cpu->e); return info->cycles;
        case 0x24: cpu->h = alu_inc(cpu, cpu->h); return info->cycles;
        case 0x2C: cpu->l = alu_inc(cpu, cpu->l); return info->cycles;
        case 0x34: mmu_wb(cpu->mmu, cpu->hl, alu_inc(cpu, mmu_rb(cpu->mmu, cpu->hl))); return info->cycles;
        case 0x3C: cpu->a = alu_inc(cpu, cpu->a); return info->cycles;

        case 0x05: cpu->b = alu_dec(cpu, cpu->b); return info->cycles;
        case 0x0D: cpu->c = alu_dec(cpu, cpu->c); return info->cycles;
        case 0x15: cpu->d = alu_dec(cpu, cpu->d); return info->cycles;
        case 0x1D: cpu->e = alu_dec(cpu, cpu->e); return info->cycles;
        case 0x25: cpu->h = alu_dec(cpu, cpu->h); return info->cycles;
        case 0x2D: cpu->l = alu_dec(cpu, cpu->l); return info->cycles;
        case 0x35: mmu_wb(cpu->mmu, cpu->hl, alu_dec(cpu, mmu_rb(cpu->mmu, cpu->hl))); return info->cycles;
        case 0x3D: cpu->a = alu_dec(cpu, cpu->a); return info->cycles;

        case 0x27: cpu->a = alu_daa(cpu, cpu->a); return info->cycles;
        case 0x2F: cpu->a = alu_cpl(cpu, cpu->a); return info->cycles;
        case 0x37: alu_scf(cpu); return info->cycles;
        case 0x3F: alu_ccf(cpu); return info->cycles;

        case 0x07: cpu->a = alu_rlc(cpu, cpu->a); cpu_set_flag(cpu, FLAG_Z, 0); return info->cycles;
        case 0x0F: cpu->a = alu_rrc(cpu, cpu->a); cpu_set_flag(cpu, FLAG_Z, 0); return info->cycles;
        case 0x17: cpu->a = alu_rl(cpu, cpu->a); cpu_set_flag(cpu, FLAG_Z, 0); return info->cycles;
        case 0x1F: cpu->a = alu_rr(cpu, cpu->a); cpu_set_flag(cpu, FLAG_Z, 0); return info->cycles;

        /* 16-Bit ALU Instructions */

        case 0x03: cpu->bc++; return info->cycles;
        case 0x13: cpu->de++; return info->cycles;
        case 0x23: cpu->hl++; return info->cycles;
        case 0x33: cpu->sp++; return info->cycles;
        case 0x0B: cpu->bc--; return info->cycles;
        case 0x1B: cpu->de--; return info->cycles;
        case 0x2B: cpu->hl--; return info->cycles;
        case 0x3B: cpu->sp--; return info->cycles;

        case 0x09: cpu->hl = alu_add16(cpu, cpu->hl, cpu->bc); return info->cycles;
        case 0x19: cpu->hl = alu_add16(cpu, cpu->hl, cpu->de); return info->cycles;
        case 0x29: cpu->hl = alu_add16(cpu, cpu->hl, cpu->hl); return info->cycles;
        case 0x39: cpu->hl = alu_add16(cpu, cpu->hl, cpu->sp); return info->cycles;

        case 0xE8: cpu->sp = alu_add_u16_i8(cpu, cpu->sp, (int8_t)cpu_fb(cpu)); return info->cycles;

        /* Illegal opcodes */
        case 0xD3:
        case 0xE3:
        case 0xE4:
        case 0xF4:
        case 0xDB:
        case 0xEB:
        case 0xEC:
        case 0xFC:
        case 0xDD:
        case 0xED:
        case 0xFD:
            fprintf(stderr, "illegal opcode: 0x%02X\n", opcode);
            return 0;
    }

    fprintf(stderr, "unimplemented opcode: 0x%02X\n", opcode);
    return 0;
}
