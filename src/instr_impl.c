#include "instr_impl.h"
#include "alu.h"
#include "mmu.h"

#if PRINT_DEBUG == 1
#define DEBUG_INSTR(...) do { printf(__VA_ARGS__); printf("\n"); } while(0)
#else
#define DEBUG_INSTR(...)
#endif

#define IHL_GET()   mmu_rb(cpu->mmu, cpu->hl)
#define IHL_SET(v)  mmu_wb(cpu->mmu, cpu->hl, (v))

static int instr_impl_0x00(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("NOP");
    (void)cpu;
    return info->cycles;
}

static int instr_impl_0x01(struct cpu *cpu, const struct instr_info *info) {
    uint16_t v = cpu_fw(cpu);
    DEBUG_INSTR("LD BC,0x%04X", v);
    cpu->bc = v;
    return info->cycles;
}

static int instr_impl_0x02(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("LD (BC),A");
    mmu_wb(cpu->mmu, cpu->bc, cpu->a);
    return info->cycles;
}

static int instr_impl_0x03(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("INC BC");
    cpu->bc++;
    return info->cycles;
}

static int instr_impl_0x04(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("INC B");
    cpu->b = alu_inc(cpu, cpu->b);
    return info->cycles;
}

static int instr_impl_0x05(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("DEC B");
    cpu->b = alu_dec(cpu, cpu->b);
    return info->cycles;
}

static int instr_impl_0x06(struct cpu *cpu, const struct instr_info *info) {
    uint8_t v = cpu_fb(cpu);
    DEBUG_INSTR("LD B,0x%02X", v);
    cpu->b = v;
    return info->cycles;
}

static int instr_impl_0x07(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("RLCA");
    cpu->a = alu_rlc(cpu, cpu->a);
    cpu->f &= ~FLAG_Z;
    return info->cycles;
}

static int instr_impl_0x08(struct cpu *cpu, const struct instr_info *info) {
    uint16_t v = cpu_fw(cpu);
    DEBUG_INSTR("LD (0x%04X),SP", v);
    mmu_ww(cpu->mmu, v, cpu->sp);
    return info->cycles;
}

static int instr_impl_0x09(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("ADD HL,BC");
    cpu->hl = alu_add16(cpu, cpu->hl, cpu->bc);
    return info->cycles;
}

static int instr_impl_0x0A(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("LD A,(BC)");
    cpu->a = mmu_rb(cpu->mmu, cpu->bc);
    return info->cycles;
}

static int instr_impl_0x0B(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("DEC BC");
    cpu->bc--;
    return info->cycles;
}

static int instr_impl_0x0C(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("INC C");
    cpu->c = alu_inc(cpu, cpu->c);
    return info->cycles;
}

static int instr_impl_0x0D(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("DEC C");
    cpu->c = alu_dec(cpu, cpu->c);
    return info->cycles;
}

static int instr_impl_0x0E(struct cpu *cpu, const struct instr_info *info) {
    uint8_t v = cpu_fb(cpu);
    DEBUG_INSTR("LD C,0x%02X", v);
    cpu->c = v;
    return info->cycles;
}

static int instr_impl_0x0F(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("RRCA");
    cpu->a = alu_rrc(cpu, cpu->a);
    cpu->f &= ~FLAG_Z;
    return info->cycles;
}

static int instr_impl_0x10(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("STOP 0");
    cpu->stop = 1;
    return info->cycles;
}

static int instr_impl_0x11(struct cpu *cpu, const struct instr_info *info) {
    uint16_t v = cpu_fw(cpu);
    DEBUG_INSTR("LD DE,0x%04X", v);
    cpu->de = v;
    return info->cycles;
}

static int instr_impl_0x12(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("LD (DE),A");
    mmu_wb(cpu->mmu, cpu->de, cpu->a);
    return info->cycles;
}

static int instr_impl_0x13(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("INC DE");
    cpu->de++;
    return info->cycles;
}

static int instr_impl_0x14(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("INC D");
    cpu->d = alu_inc(cpu, cpu->d);
    return info->cycles;
}

static int instr_impl_0x15(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("DEC D");
    cpu->d = alu_dec(cpu, cpu->d);
    return info->cycles;
}

static int instr_impl_0x16(struct cpu *cpu, const struct instr_info *info) {
    uint8_t v = cpu_fb(cpu);
    DEBUG_INSTR("LD D,0x%02X", v);
    cpu->d = v;
    return info->cycles;
}

static int instr_impl_0x17(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("RLA");
    cpu->a = alu_rl(cpu, cpu->a);
    cpu->f &= ~FLAG_Z;
    return info->cycles;
}

static int instr_impl_0x18(struct cpu *cpu, const struct instr_info *info) {
    int8_t v = (int8_t)cpu_fb(cpu);
    DEBUG_INSTR("JR 0x%04X", cpu->pc + v);
    cpu->pc = (uint16_t)(cpu->pc + v);
    return info->cycles;
}

static int instr_impl_0x19(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("ADD HL,DE");
    cpu->hl = alu_add16(cpu, cpu->hl, cpu->de);
    return info->cycles;
}

static int instr_impl_0x1A(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("LD A,(DE)");
    cpu->a = mmu_rb(cpu->mmu, cpu->de);
    return info->cycles;
}

static int instr_impl_0x1B(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("DEC DE");
    cpu->de--;
    return info->cycles;
}

static int instr_impl_0x1C(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("INC E");
    cpu->e = alu_inc(cpu, cpu->e);
    return info->cycles;
}

static int instr_impl_0x1D(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("DEC E");
    cpu->e = alu_dec(cpu, cpu->e);
    return info->cycles;
}

static int instr_impl_0x1E(struct cpu *cpu, const struct instr_info *info) {
    uint8_t v = cpu_fb(cpu);
    DEBUG_INSTR("LD E,0x%02X", v);
    cpu->e = v;
    return info->cycles;
}

static int instr_impl_0x1F(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("RRA");
    cpu->a = alu_rr(cpu, cpu->a);
    cpu->f &= ~FLAG_Z;
    return info->cycles;
}

static int instr_impl_0x20(struct cpu *cpu, const struct instr_info *info) {
    int8_t v = (int8_t)cpu_fb(cpu);
    DEBUG_INSTR("JR NZ,0x%04X", cpu->pc + v);
    if((cpu->f & FLAG_Z) == 0) {
        cpu->pc = (uint16_t)(cpu->pc + v);
        return info->cond_cycles;
    }
    return info->cycles;
}

static int instr_impl_0x21(struct cpu *cpu, const struct instr_info *info) {
    uint16_t v = cpu_fw(cpu);
    DEBUG_INSTR("LD HL,0x%04X", v);
    cpu->hl = v;
    return info->cycles;
}

static int instr_impl_0x22(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("LD (HL+),A");
    IHL_SET(cpu->a);
    cpu->hl++;
    return info->cycles;
}

static int instr_impl_0x23(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("INC HL");
    cpu->hl++;
    return info->cycles;
}

static int instr_impl_0x24(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("INC H");
    cpu->h = alu_inc(cpu, cpu->h);
    return info->cycles;
}

static int instr_impl_0x25(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("DEC H");
    cpu->h = alu_dec(cpu, cpu->h);
    return info->cycles;
}

static int instr_impl_0x26(struct cpu *cpu, const struct instr_info *info) {
    uint8_t v = cpu_fb(cpu);
    DEBUG_INSTR("LD H,0x%02X", v);
    cpu->h = v;
    return info->cycles;
}

static int instr_impl_0x27(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("DAA");
    cpu->a = alu_daa(cpu, cpu->a);
    return info->cycles;
}

static int instr_impl_0x28(struct cpu *cpu, const struct instr_info *info) {
    int8_t v = (int8_t)cpu_fb(cpu);
    DEBUG_INSTR("JR Z,0x%04X", cpu->pc + v);
    if((cpu->f & FLAG_Z) != 0) {
        cpu->pc = (uint16_t)(cpu->pc + v);
        return info->cond_cycles;
    }
    return info->cycles;
}

static int instr_impl_0x29(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("ADD HL,HL");
    cpu->hl = alu_add16(cpu, cpu->hl, cpu->hl);
    return info->cycles;
}

static int instr_impl_0x2A(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("LD A,(HL+)");
    cpu->a = IHL_GET();
    cpu->hl++;
    return info->cycles;
}

static int instr_impl_0x2B(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("DEC HL");
    cpu->hl--;
    return info->cycles;
}

static int instr_impl_0x2C(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("INC L");
    cpu->l = alu_inc(cpu, cpu->l);
    return info->cycles;
}

static int instr_impl_0x2D(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("DEC L");
    cpu->l = alu_dec(cpu, cpu->l);
    return info->cycles;
}

static int instr_impl_0x2E(struct cpu *cpu, const struct instr_info *info) {
    uint8_t v = cpu_fb(cpu);
    DEBUG_INSTR("LD L,0x%02X", v);
    cpu->l = v;
    return info->cycles;
}

static int instr_impl_0x2F(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("CPL");
    cpu->a = alu_cpl(cpu, cpu->a);
    return info->cycles;
}

static int instr_impl_0x30(struct cpu *cpu, const struct instr_info *info) {
    int8_t v = (int8_t)cpu_fb(cpu);
    DEBUG_INSTR("JR NC,0x%04X", cpu->pc + v);
    if((cpu->f & FLAG_C) == 0) {
        cpu->pc = (uint16_t)(cpu->pc + v);
        return info->cond_cycles;
    }
    return info->cycles;
}

static int instr_impl_0x31(struct cpu *cpu, const struct instr_info *info) {
    uint16_t v = cpu_fw(cpu);
    DEBUG_INSTR("LD SP,0x%04X", v);
    cpu->sp = v;
    return info->cycles;
}

static int instr_impl_0x32(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("LD (HL-),A");
    IHL_SET(cpu->a);
    cpu->hl--;
    return info->cycles;
}

static int instr_impl_0x33(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("INC SP");
    cpu->sp++;
    return info->cycles;
}

static int instr_impl_0x34(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("INC (HL)");
    IHL_SET(alu_inc(cpu, IHL_GET()));
    return info->cycles;
}

static int instr_impl_0x35(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("DEC (HL)");
    IHL_SET(alu_dec(cpu, IHL_GET()));
    return info->cycles;
}

static int instr_impl_0x36(struct cpu *cpu, const struct instr_info *info) {
    uint8_t v = cpu_fb(cpu);
    DEBUG_INSTR("LD (HL),0x%02X", v);
    IHL_SET(v);
    return info->cycles;
}

static int instr_impl_0x37(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("SCF");
    alu_scf(cpu);
    return info->cycles;
}

static int instr_impl_0x38(struct cpu *cpu, const struct instr_info *info) {
    int8_t v = (int8_t)cpu_fb(cpu);
    DEBUG_INSTR("JR C,0x%04X", cpu->pc + v);
    if((cpu->f & FLAG_C) != 0) {
        cpu->pc = (uint16_t)(cpu->pc + v);
        return info->cond_cycles;
    }
    return info->cycles;
}

static int instr_impl_0x39(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("ADD HL,SP");
    cpu->hl = alu_add16(cpu, cpu->hl, cpu->sp);
    return info->cycles;
}

static int instr_impl_0x3A(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("LD A,(HL-)");
    cpu->a = IHL_GET();
    cpu->hl--;
    return info->cycles;
}

static int instr_impl_0x3B(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("DEC SP");
    cpu->sp--;
    return info->cycles;
}

static int instr_impl_0x3C(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("INC A");
    cpu->a = alu_inc(cpu, cpu->a);
    return info->cycles;
}

static int instr_impl_0x3D(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("DEC A");
    cpu->a = alu_dec(cpu, cpu->a);
    return info->cycles;
}

static int instr_impl_0x3E(struct cpu *cpu, const struct instr_info *info) {
    uint8_t v = cpu_fb(cpu);
    DEBUG_INSTR("LD A,0x%02X", v);
    cpu->a = v;
    return info->cycles;
}

static int instr_impl_0x3F(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("CCF");
    alu_ccf(cpu);
    return info->cycles;
}

static int instr_impl_0x40(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("LD B,B");
    cpu->b = cpu->b;
    return info->cycles;
}

static int instr_impl_0x41(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("LD B,C");
    cpu->b = cpu->c;
    return info->cycles;
}

static int instr_impl_0x42(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("LD B,D");
    cpu->b = cpu->d;
    return info->cycles;
}

static int instr_impl_0x43(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("LD B,E");
    cpu->b = cpu->e;
    return info->cycles;
}

static int instr_impl_0x44(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("LD B,H");
    cpu->b = cpu->h;
    return info->cycles;
}

static int instr_impl_0x45(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("LD B,L");
    cpu->b = cpu->l;
    return info->cycles;
}

static int instr_impl_0x46(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("LD B,(HL)");
    cpu->b = IHL_GET();
    return info->cycles;
}

static int instr_impl_0x47(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("LD B,A");
    cpu->b = cpu->a;
    return info->cycles;
}

static int instr_impl_0x48(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("LD C,B");
    cpu->c = cpu->b;
    return info->cycles;
}

static int instr_impl_0x49(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("LD C,C");
    cpu->c = cpu->c;
    return info->cycles;
}

static int instr_impl_0x4A(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("LD C,D");
    cpu->c = cpu->d;
    return info->cycles;
}

static int instr_impl_0x4B(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("LD C,E");
    cpu->c = cpu->e;
    return info->cycles;
}

static int instr_impl_0x4C(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("LD C,H");
    cpu->c = cpu->h;
    return info->cycles;
}

static int instr_impl_0x4D(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("LD C,L");
    cpu->c = cpu->l;
    return info->cycles;
}

static int instr_impl_0x4E(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("LD C,(HL)");
    cpu->c = IHL_GET();
    return info->cycles;
}

static int instr_impl_0x4F(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("LD C,A");
    cpu->c = cpu->a;
    return info->cycles;
}

static int instr_impl_0x50(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("LD D,B");
    cpu->d = cpu->b;
    return info->cycles;
}

static int instr_impl_0x51(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("LD D,C");
    cpu->d = cpu->c;
    return info->cycles;
}

static int instr_impl_0x52(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("LD D,D");
    cpu->d = cpu->d;
    return info->cycles;
}

static int instr_impl_0x53(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("LD D,E");
    cpu->d = cpu->e;
    return info->cycles;
}

static int instr_impl_0x54(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("LD D,H");
    cpu->d = cpu->h;
    return info->cycles;
}

static int instr_impl_0x55(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("LD D,L");
    cpu->d = cpu->l;
    return info->cycles;
}

static int instr_impl_0x56(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("LD D,(HL)");
    cpu->d = IHL_GET();
    return info->cycles;
}

static int instr_impl_0x57(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("LD D,A");
    cpu->d = cpu->a;
    return info->cycles;
}

static int instr_impl_0x58(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("LD E,B");
    cpu->e = cpu->b;
    return info->cycles;
}

static int instr_impl_0x59(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("LD E,C");
    cpu->e = cpu->c;
    return info->cycles;
}

static int instr_impl_0x5A(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("LD E,D");
    cpu->e = cpu->d;
    return info->cycles;
}

static int instr_impl_0x5B(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("LD E,E");
    cpu->e = cpu->e;
    return info->cycles;
}

static int instr_impl_0x5C(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("LD E,H");
    cpu->e = cpu->h;
    return info->cycles;
}

static int instr_impl_0x5D(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("LD E,L");
    cpu->e = cpu->l;
    return info->cycles;
}

static int instr_impl_0x5E(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("LD E,(HL)");
    cpu->e = IHL_GET();
    return info->cycles;
}

static int instr_impl_0x5F(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("LD E,A");
    cpu->e = cpu->a;
    return info->cycles;
}

static int instr_impl_0x60(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("LD H,B");
    cpu->h = cpu->b;
    return info->cycles;
}

static int instr_impl_0x61(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("LD H,C");
    cpu->h = cpu->c;
    return info->cycles;
}

static int instr_impl_0x62(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("LD H,D");
    cpu->h = cpu->d;
    return info->cycles;
}

static int instr_impl_0x63(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("LD H,E");
    cpu->h = cpu->e;
    return info->cycles;
}

static int instr_impl_0x64(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("LD H,H");
    cpu->h = cpu->h;
    return info->cycles;
}

static int instr_impl_0x65(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("LD H,L");
    cpu->h = cpu->l;
    return info->cycles;
}

static int instr_impl_0x66(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("LD H,(HL)");
    cpu->h = IHL_GET();
    return info->cycles;
}

static int instr_impl_0x67(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("LD H,A");
    cpu->h = cpu->a;
    return info->cycles;
}

static int instr_impl_0x68(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("LD L,B");
    cpu->l = cpu->b;
    return info->cycles;
}

static int instr_impl_0x69(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("LD L,C");
    cpu->l = cpu->c;
    return info->cycles;
}

static int instr_impl_0x6A(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("LD L,D");
    cpu->l = cpu->d;
    return info->cycles;
}

static int instr_impl_0x6B(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("LD L,E");
    cpu->l = cpu->e;
    return info->cycles;
}

static int instr_impl_0x6C(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("LD L,H");
    cpu->l = cpu->h;
    return info->cycles;
}

static int instr_impl_0x6D(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("LD L,L");
    cpu->l = cpu->l;
    return info->cycles;
}

static int instr_impl_0x6E(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("LD L,(HL)");
    cpu->l = IHL_GET();
    return info->cycles;
}

static int instr_impl_0x6F(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("LD L,A");
    cpu->l = cpu->a;
    return info->cycles;
}

static int instr_impl_0x70(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("LD (HL),B");
    IHL_SET(cpu->b);
    return info->cycles;
}

static int instr_impl_0x71(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("LD (HL),C");
    IHL_SET(cpu->c);
    return info->cycles;
}

static int instr_impl_0x72(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("LD (HL),D");
    IHL_SET(cpu->d);
    return info->cycles;
}

static int instr_impl_0x73(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("LD (HL),E");
    IHL_SET(cpu->e);
    return info->cycles;
}

static int instr_impl_0x74(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("LD (HL),H");
    IHL_SET(cpu->h);
    return info->cycles;
}

static int instr_impl_0x75(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("LD (HL),L");
    IHL_SET(cpu->l);
    return info->cycles;
}

static int instr_impl_0x76(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("HALT");
    cpu->halt = 1;
    return info->cycles;
}

static int instr_impl_0x77(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("LD (HL),A");
    IHL_SET(cpu->a);
    return info->cycles;
}

static int instr_impl_0x78(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("LD A,B");
    cpu->a = cpu->b;
    return info->cycles;
}

static int instr_impl_0x79(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("LD A,C");
    cpu->a = cpu->c;
    return info->cycles;
}

static int instr_impl_0x7A(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("LD A,D");
    cpu->a = cpu->d;
    return info->cycles;
}

static int instr_impl_0x7B(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("LD A,E");
    cpu->a = cpu->e;
    return info->cycles;
}

static int instr_impl_0x7C(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("LD A,H");
    cpu->a = cpu->h;
    return info->cycles;
}

static int instr_impl_0x7D(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("LD A,L");
    cpu->a = cpu->l;
    return info->cycles;
}

static int instr_impl_0x7E(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("LD A,(HL)");
    cpu->a = IHL_GET();
    return info->cycles;
}

static int instr_impl_0x7F(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("LD A,A");
    cpu->a = cpu->a;
    return info->cycles;
}

static int instr_impl_0x80(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("ADD A,B");
    cpu->a = alu_add(cpu, cpu->a, cpu->b);
    return info->cycles;
}

static int instr_impl_0x81(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("ADD A,C");
    cpu->a = alu_add(cpu, cpu->a, cpu->c);
    return info->cycles;
}

static int instr_impl_0x82(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("ADD A,D");
    cpu->a = alu_add(cpu, cpu->a, cpu->d);
    return info->cycles;
}

static int instr_impl_0x83(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("ADD A,E");
    cpu->a = alu_add(cpu, cpu->a, cpu->e);
    return info->cycles;
}

static int instr_impl_0x84(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("ADD A,H");
    cpu->a = alu_add(cpu, cpu->a, cpu->h);
    return info->cycles;
}

static int instr_impl_0x85(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("ADD A,L");
    cpu->a = alu_add(cpu, cpu->a, cpu->l);
    return info->cycles;
}

static int instr_impl_0x86(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("ADD A,(HL)");
    cpu->a = alu_add(cpu, cpu->a, IHL_GET());
    return info->cycles;
}

static int instr_impl_0x87(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("ADD A,A");
    cpu->a = alu_add(cpu, cpu->a, cpu->a);
    return info->cycles;
}

static int instr_impl_0x88(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("ADC A,B");
    cpu->a = alu_adc(cpu, cpu->a, cpu->b);
    return info->cycles;
}

static int instr_impl_0x89(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("ADC A,C");
    cpu->a = alu_adc(cpu, cpu->a, cpu->c);
    return info->cycles;
}

static int instr_impl_0x8A(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("ADC A,D");
    cpu->a = alu_adc(cpu, cpu->a, cpu->d);
    return info->cycles;
}

static int instr_impl_0x8B(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("ADC A,E");
    cpu->a = alu_adc(cpu, cpu->a, cpu->e);
    return info->cycles;
}

static int instr_impl_0x8C(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("ADC A,H");
    cpu->a = alu_adc(cpu, cpu->a, cpu->h);
    return info->cycles;
}

static int instr_impl_0x8D(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("ADC A,L");
    cpu->a = alu_adc(cpu, cpu->a, cpu->l);
    return info->cycles;
}

static int instr_impl_0x8E(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("ADC A,(HL)");
    cpu->a = alu_adc(cpu, cpu->a, IHL_GET());
    return info->cycles;
}

static int instr_impl_0x8F(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("ADC A,A");
    cpu->a = alu_adc(cpu, cpu->a, cpu->a);
    return info->cycles;
}

static int instr_impl_0x90(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("SUB B");
    cpu->a = alu_sub(cpu, cpu->a, cpu->b);
    return info->cycles;
}

static int instr_impl_0x91(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("SUB C");
    cpu->a = alu_sub(cpu, cpu->a, cpu->c);
    return info->cycles;
}

static int instr_impl_0x92(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("SUB D");
    cpu->a = alu_sub(cpu, cpu->a, cpu->d);
    return info->cycles;
}

static int instr_impl_0x93(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("SUB E");
    cpu->a = alu_sub(cpu, cpu->a, cpu->e);
    return info->cycles;
}

static int instr_impl_0x94(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("SUB H");
    cpu->a = alu_sub(cpu, cpu->a, cpu->h);
    return info->cycles;
}

static int instr_impl_0x95(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("SUB L");
    cpu->a = alu_sub(cpu, cpu->a, cpu->l);
    return info->cycles;
}

static int instr_impl_0x96(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("SUB (HL)");
    cpu->a = alu_sub(cpu, cpu->a, IHL_GET());
    return info->cycles;
}

static int instr_impl_0x97(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("SUB A");
    cpu->a = alu_sub(cpu, cpu->a, cpu->a);
    return info->cycles;
}

static int instr_impl_0x98(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("SBC A,B");
    cpu->a = alu_sbc(cpu, cpu->a, cpu->b);
    return info->cycles;
}

static int instr_impl_0x99(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("SBC A,C");
    cpu->a = alu_sbc(cpu, cpu->a, cpu->c);
    return info->cycles;
}

static int instr_impl_0x9A(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("SBC A,D");
    cpu->a = alu_sbc(cpu, cpu->a, cpu->d);
    return info->cycles;
}

static int instr_impl_0x9B(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("SBC A,E");
    cpu->a = alu_sbc(cpu, cpu->a, cpu->e);
    return info->cycles;
}

static int instr_impl_0x9C(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("SBC A,H");
    cpu->a = alu_sbc(cpu, cpu->a, cpu->h);
    return info->cycles;
}

static int instr_impl_0x9D(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("SBC A,L");
    cpu->a = alu_sbc(cpu, cpu->a, cpu->l);
    return info->cycles;
}

static int instr_impl_0x9E(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("SBC A,(HL)");
    cpu->a = alu_sbc(cpu, cpu->a, IHL_GET());
    return info->cycles;
}

static int instr_impl_0x9F(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("SBC A,A");
    cpu->a = alu_sbc(cpu, cpu->a, cpu->a);
    return info->cycles;
}

static int instr_impl_0xA0(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("AND B");
    cpu->a = alu_and(cpu, cpu->a, cpu->b);
    return info->cycles;
}

static int instr_impl_0xA1(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("AND C");
    cpu->a = alu_and(cpu, cpu->a, cpu->c);
    return info->cycles;
}

static int instr_impl_0xA2(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("AND D");
    cpu->a = alu_and(cpu, cpu->a, cpu->d);
    return info->cycles;
}

static int instr_impl_0xA3(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("AND E");
    cpu->a = alu_and(cpu, cpu->a, cpu->e);
    return info->cycles;
}

static int instr_impl_0xA4(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("AND H");
    cpu->a = alu_and(cpu, cpu->a, cpu->h);
    return info->cycles;
}

static int instr_impl_0xA5(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("AND L");
    cpu->a = alu_and(cpu, cpu->a, cpu->l);
    return info->cycles;
}

static int instr_impl_0xA6(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("AND (HL)");
    cpu->a = alu_and(cpu, cpu->a, IHL_GET());
    return info->cycles;
}

static int instr_impl_0xA7(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("AND A");
    cpu->a = alu_and(cpu, cpu->a, cpu->a);
    return info->cycles;
}

static int instr_impl_0xA8(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("XOR B");
    cpu->a = alu_xor(cpu, cpu->a, cpu->b);
    return info->cycles;
}

static int instr_impl_0xA9(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("XOR C");
    cpu->a = alu_xor(cpu, cpu->a, cpu->c);
    return info->cycles;
}

static int instr_impl_0xAA(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("XOR D");
    cpu->a = alu_xor(cpu, cpu->a, cpu->d);
    return info->cycles;
}

static int instr_impl_0xAB(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("XOR E");
    cpu->a = alu_xor(cpu, cpu->a, cpu->e);
    return info->cycles;
}

static int instr_impl_0xAC(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("XOR H");
    cpu->a = alu_xor(cpu, cpu->a, cpu->h);
    return info->cycles;
}

static int instr_impl_0xAD(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("XOR L");
    cpu->a = alu_xor(cpu, cpu->a, cpu->l);
    return info->cycles;
}

static int instr_impl_0xAE(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("XOR (HL)");
    cpu->a = alu_xor(cpu, cpu->a, IHL_GET());
    return info->cycles;
}

static int instr_impl_0xAF(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("XOR A");
    cpu->a = alu_xor(cpu, cpu->a, cpu->a);
    return info->cycles;
}

static int instr_impl_0xB0(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("OR B");
    cpu->a = alu_or(cpu, cpu->a, cpu->b);
    return info->cycles;
}

static int instr_impl_0xB1(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("OR C");
    cpu->a = alu_or(cpu, cpu->a, cpu->c);
    return info->cycles;
}

static int instr_impl_0xB2(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("OR D");
    cpu->a = alu_or(cpu, cpu->a, cpu->d);
    return info->cycles;
}

static int instr_impl_0xB3(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("OR E");
    cpu->a = alu_or(cpu, cpu->a, cpu->e);
    return info->cycles;
}

static int instr_impl_0xB4(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("OR H");
    cpu->a = alu_or(cpu, cpu->a, cpu->h);
    return info->cycles;
}

static int instr_impl_0xB5(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("OR L");
    cpu->a = alu_or(cpu, cpu->a, cpu->l);
    return info->cycles;
}

static int instr_impl_0xB6(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("OR (HL)");
    cpu->a = alu_or(cpu, cpu->a, IHL_GET());
    return info->cycles;
}

static int instr_impl_0xB7(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("OR A");
    cpu->a = alu_or(cpu, cpu->a, cpu->a);
    return info->cycles;
}

static int instr_impl_0xB8(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("CP B");
    alu_cp(cpu, cpu->a, cpu->b);
    return info->cycles;
}

static int instr_impl_0xB9(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("CP C");
    alu_cp(cpu, cpu->a, cpu->c);
    return info->cycles;
}

static int instr_impl_0xBA(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("CP D");
    alu_cp(cpu, cpu->a, cpu->d);
    return info->cycles;
}

static int instr_impl_0xBB(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("CP E");
    alu_cp(cpu, cpu->a, cpu->e);
    return info->cycles;
}

static int instr_impl_0xBC(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("CP H");
    alu_cp(cpu, cpu->a, cpu->h);
    return info->cycles;
}

static int instr_impl_0xBD(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("CP L");
    alu_cp(cpu, cpu->a, cpu->l);
    return info->cycles;
}

static int instr_impl_0xBE(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("CP (HL)");
    alu_cp(cpu, cpu->a, IHL_GET());
    return info->cycles;
}

static int instr_impl_0xBF(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("CP A");
    alu_cp(cpu, cpu->a, cpu->a);
    return info->cycles;
}

static int instr_impl_0xC0(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("RET NZ");
    if((cpu->f & FLAG_Z) == 0) {
        cpu->pc = cpu_pop(cpu);
        return info->cond_cycles;
    }
    return info->cycles;
}

static int instr_impl_0xC1(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("POP BC");
    cpu->bc = cpu_pop(cpu);
    return info->cycles;
}

static int instr_impl_0xC2(struct cpu *cpu, const struct instr_info *info) {
    uint16_t v = cpu_fw(cpu);
    DEBUG_INSTR("JP NZ,0x%04X", v);
    if(!cpu_flag(cpu, FLAG_Z)) {
        cpu->pc = v;
        return info->cycles;
    }
    return info->cycles;
}

static int instr_impl_0xC3(struct cpu *cpu, const struct instr_info *info) {
    uint16_t v = cpu_fw(cpu);
    DEBUG_INSTR("JP 0x%04X", v);
    cpu->pc = v;
    return info->cycles;
}

static int instr_impl_0xC4(struct cpu *cpu, const struct instr_info *info) {
    uint16_t v = cpu_fw(cpu);
    DEBUG_INSTR("CALL NZ,0x%04X", v);
    if((cpu->f & FLAG_Z) == 0) {
        cpu_push(cpu, cpu->pc);
        cpu->pc = v;
        return info->cond_cycles;
    }
    return info->cycles;
}

static int instr_impl_0xC5(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("PUSH BC");
    cpu_push(cpu, cpu->bc);
    return info->cycles;
}

static int instr_impl_0xC6(struct cpu *cpu, const struct instr_info *info) {
    uint8_t v = cpu_fb(cpu);
    DEBUG_INSTR("ADD A,0x%02X", v);
    cpu->a = alu_add(cpu, cpu->a, v);
    return info->cycles;
}

static int instr_impl_0xC7(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("RST 00H");
    cpu_push(cpu, cpu->pc);
    cpu->pc = 0x0000;
    return info->cycles;
}

static int instr_impl_0xC8(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("RET Z");
    if((cpu->f & FLAG_Z) != 0) {
        cpu->pc = cpu_pop(cpu);
        return info->cond_cycles;
    }
    return info->cycles;
}

static int instr_impl_0xC9(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("RET");
    cpu->pc = cpu_pop(cpu);
    return info->cycles;
}

static int instr_impl_0xCA(struct cpu *cpu, const struct instr_info *info) {
    uint16_t v = cpu_fw(cpu);
    DEBUG_INSTR("JP Z,0x%04X", v);
    if((cpu->f & FLAG_Z) != 0) {
        cpu->pc = v;
        return info->cycles;
    }
    return info->cycles;
}

static int instr_impl_0xCB(struct cpu *cpu, const struct instr_info *info) {
    (void)info;
    uint8_t opcode = cpu_fb(cpu);
    return INSTR_IMPL_PREFIX[opcode](cpu, &INSTR_INFO_PREFIX[opcode]);
}

static int instr_impl_0xCC(struct cpu *cpu, const struct instr_info *info) {
    uint16_t v = cpu_fw(cpu);
    DEBUG_INSTR("CALL Z,0x%04X", v);
    if(cpu->f & FLAG_Z) {
        cpu_push(cpu, cpu->pc);
        cpu->pc = v;
        return info->cond_cycles;
    }
    return info->cycles;
}

static int instr_impl_0xCD(struct cpu *cpu, const struct instr_info *info) {
    uint16_t v = cpu_fw(cpu);
    DEBUG_INSTR("CALL 0x%04X", v);
    cpu_push(cpu, cpu->pc);
    cpu->pc = v;
    return info->cycles;
}

static int instr_impl_0xCE(struct cpu *cpu, const struct instr_info *info) {
    uint8_t v = cpu_fb(cpu);
    DEBUG_INSTR("ADC A,0x%02X", v);
    cpu->a = alu_adc(cpu, cpu->a, v);
    return info->cycles;
}

static int instr_impl_0xCF(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("RST 08H");
    cpu_push(cpu, cpu->pc);
    cpu->pc = 0x0008;
    return info->cycles;
}

static int instr_impl_0xD0(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("RET NC");
    if((cpu->f & FLAG_C) == 0) {
        cpu->pc = cpu_pop(cpu);
        return info->cond_cycles;
    }
    return info->cycles;
}

static int instr_impl_0xD1(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("POP DE");
    cpu->de = cpu_pop(cpu);
    return info->cycles;
}

static int instr_impl_0xD2(struct cpu *cpu, const struct instr_info *info) {
    uint16_t v = cpu_fw(cpu);
    DEBUG_INSTR("JP NC,0x%04X", v);
    if((cpu->f & FLAG_C) == 0) {
        cpu->pc = v;
        return info->cond_cycles;
    }
    return info->cycles;
}

static int instr_impl_0xD3(struct cpu *cpu, const struct instr_info *info) {
    (void)cpu;
    fprintf(stderr, "illegal opcode: 0x%02X\n", info->opcode);
    cpu->running = 0;
    return info->cycles;
}

static int instr_impl_0xD4(struct cpu *cpu, const struct instr_info *info) {
    uint16_t v = cpu_fw(cpu);
    DEBUG_INSTR("CALL NC,0x%04X", v);
    if(!(cpu->f & FLAG_C)) {
        cpu_push(cpu, cpu->pc);
        cpu->pc = v;
        return info->cond_cycles;
    }
    return info->cycles;
}

static int instr_impl_0xD5(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("PUSH DE");
    cpu_push(cpu, cpu->de);
    return info->cycles;
}

static int instr_impl_0xD6(struct cpu *cpu, const struct instr_info *info) {
    uint8_t v = cpu_fb(cpu);
    DEBUG_INSTR("SUB 0x%02X", v);
    cpu->a = alu_sub(cpu, cpu->a, v);
    return info->cycles;
}

static int instr_impl_0xD7(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("RST 10H");
    cpu_push(cpu, cpu->pc);
    cpu->pc = 0x0010;
    return info->cycles;
}

static int instr_impl_0xD8(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("RET C");
    if(cpu->f & FLAG_C) {
        cpu->pc = cpu_pop(cpu);
        return info->cond_cycles;
    }
    return info->cycles;
}

static int instr_impl_0xD9(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("RETI");
    cpu->pc = cpu_pop(cpu);
    cpu->ime = 1;
    return info->cycles;
}

static int instr_impl_0xDA(struct cpu *cpu, const struct instr_info *info) {
    uint16_t v = cpu_fw(cpu);
    DEBUG_INSTR("JP C,0x%04X", v);
    if(cpu->f & FLAG_C) {
        cpu->pc = v;
        return info->cycles;
    }
    return info->cycles;
}

static int instr_impl_0xDB(struct cpu *cpu, const struct instr_info *info) {
    (void)cpu;
    fprintf(stderr, "illegal opcode: 0x%02X\n", info->opcode);
    cpu->running = 0;
    return info->cycles;
}

static int instr_impl_0xDC(struct cpu *cpu, const struct instr_info *info) {
    uint16_t v = cpu_fw(cpu);
    DEBUG_INSTR("CALL C,0x%04X", v);
    if(cpu->f & FLAG_C) {
        cpu_push(cpu, cpu->pc);
        cpu->pc = v;
        return info->cond_cycles;
    }
    return info->cycles;
}

static int instr_impl_0xDD(struct cpu *cpu, const struct instr_info *info) {
    (void)cpu;
    fprintf(stderr, "illegal opcode: 0x%02X\n", info->opcode);
    cpu->running = 0;
    return info->cycles;
}

static int instr_impl_0xDE(struct cpu *cpu, const struct instr_info *info) {
    uint8_t v = cpu_fb(cpu);
    DEBUG_INSTR("SBC A,0x%02X", v);
    cpu->a = alu_sbc(cpu, cpu->a, v);
    return info->cycles;
}

static int instr_impl_0xDF(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("RST 18H");
    cpu_push(cpu, cpu->pc);
    cpu->pc = 0x0018;
    return info->cycles;
}

static int instr_impl_0xE0(struct cpu *cpu, const struct instr_info *info) {
    uint8_t v = cpu_fb(cpu);
    DEBUG_INSTR("LDH (0xFF00+0x%02X),A", v);
    mmu_wb(cpu->mmu, 0xFF00 + v, cpu->a);
    return info->cycles;
}

static int instr_impl_0xE1(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("POP HL");
    cpu->hl = cpu_pop(cpu);
    return info->cycles;
}

static int instr_impl_0xE2(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("LD (C),A");
    mmu_wb(cpu->mmu, 0xFF00 + cpu->c, cpu->a);
    return info->cycles;
}

static int instr_impl_0xE3(struct cpu *cpu, const struct instr_info *info) {
    (void)cpu;
    fprintf(stderr, "illegal opcode: 0x%02X\n", info->opcode);
    cpu->running = 0;
    return info->cycles;
}

static int instr_impl_0xE4(struct cpu *cpu, const struct instr_info *info) {
    (void)cpu;
    fprintf(stderr, "illegal opcode: 0x%02X\n", info->opcode);
    cpu->running = 0;
    return info->cycles;
}

static int instr_impl_0xE5(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("PUSH HL");
    cpu_push(cpu, cpu->hl);
    return info->cycles;
}

static int instr_impl_0xE6(struct cpu *cpu, const struct instr_info *info) {
    uint8_t v = cpu_fb(cpu);
    DEBUG_INSTR("AND 0x%02X", v);
    cpu->a = alu_and(cpu, cpu->a, v);
    return info->cycles;
}

static int instr_impl_0xE7(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("RST 20H");
    cpu_push(cpu, cpu->pc);
    cpu->pc = 0x0020;
    return info->cycles;
}

static int instr_impl_0xE8(struct cpu *cpu, const struct instr_info *info) {
    int8_t v = (int8_t)cpu_fb(cpu);
    DEBUG_INSTR("ADD SP,0x%04X", v);
    cpu->sp = (uint16_t)(cpu->sp + v);
    return info->cycles;
}

static int instr_impl_0xE9(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("JP (HL)");
    cpu->pc = cpu->hl;
    return info->cycles;
}

static int instr_impl_0xEA(struct cpu *cpu, const struct instr_info *info) {
    uint16_t v = cpu_fw(cpu);
    DEBUG_INSTR("LD (0x%04X),A", v);
    mmu_wb(cpu->mmu, v, cpu->a);
    return info->cycles;
}

static int instr_impl_0xEB(struct cpu *cpu, const struct instr_info *info) {
    fprintf(stderr, "illegal opcode: 0x%02X\n", info->opcode);
    cpu->running = 0;
    return info->cycles;
}

static int instr_impl_0xEC(struct cpu *cpu, const struct instr_info *info) {
    fprintf(stderr, "illegal opcode: 0x%02X\n", info->opcode);
    cpu->running = 0;
    return info->cycles;
}

static int instr_impl_0xED(struct cpu *cpu, const struct instr_info *info) {
    fprintf(stderr, "illegal opcode: 0x%02X\n", info->opcode);
    cpu->running = 0;
    return info->cycles;
}

static int instr_impl_0xEE(struct cpu *cpu, const struct instr_info *info) {
    uint8_t v = cpu_fb(cpu);
    DEBUG_INSTR("XOR 0x%02X", v);
    cpu->a = alu_xor(cpu, cpu->a, v);
    return info->cycles;
}

static int instr_impl_0xEF(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("RST 28H");
    cpu_push(cpu, cpu->pc);
    cpu->pc = 0x0028;
    return info->cycles;
}

static int instr_impl_0xF0(struct cpu *cpu, const struct instr_info *info) {
    uint8_t v = cpu_fb(cpu);
    DEBUG_INSTR("LDH A,(0xFF00+0x%02X)", v);
    cpu->a = mmu_rb(cpu->mmu, 0xFF00 + v);
    return info->cycles;
}

static int instr_impl_0xF1(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("POP AF");
    cpu->af = cpu_pop(cpu);
    return info->cycles;
}

static int instr_impl_0xF2(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("LD A,(C)");
    cpu->a = mmu_rb(cpu->mmu, 0xFF00 + cpu->c);
    return info->cycles;
}

static int instr_impl_0xF3(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("DI");
    cpu->ime = 0;
    return info->cycles;
}

static int instr_impl_0xF4(struct cpu *cpu, const struct instr_info *info) {
    fprintf(stderr, "illegal opcode: 0x%02X\n", info->opcode);
    cpu->running = 0;
    return info->cycles;
}

static int instr_impl_0xF5(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("PUSH AF");
    cpu_push(cpu, cpu->af);
    return info->cycles;
}

static int instr_impl_0xF6(struct cpu *cpu, const struct instr_info *info) {
    uint8_t v = cpu_fb(cpu);
    DEBUG_INSTR("OR 0x%02X", v);
    cpu->a = alu_or(cpu, cpu->a, v);
    return info->cycles;
}

static int instr_impl_0xF7(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("RST 30H");
    cpu_push(cpu, cpu->pc);
    cpu->pc = 0x0030;
    return info->cycles;
}

static int instr_impl_0xF8(struct cpu *cpu, const struct instr_info *info) {
    int8_t v = (int8_t)cpu_fb(cpu);
    DEBUG_INSTR("LDHL SP,0x%02X", v);
    cpu->hl = alu_add_u16_i8(cpu, cpu->sp, v);
    return info->cycles;
}

static int instr_impl_0xF9(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("LD SP,HL");
    cpu->sp = cpu->hl;
    return info->cycles;
}

static int instr_impl_0xFA(struct cpu *cpu, const struct instr_info *info) {
    uint16_t v = cpu_fw(cpu);
    DEBUG_INSTR("LD A,(0x%04X)", v);
    cpu->a = mmu_rb(cpu->mmu, v);
    return info->cycles;
}

static int instr_impl_0xFB(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("EI");
    cpu->ime = 1;
    return info->cycles;
}

static int instr_impl_0xFC(struct cpu *cpu, const struct instr_info *info) {
    fprintf(stderr, "illegal opcode: 0x%02X\n", info->opcode);
    cpu->running = 0;
    return info->cycles;
}

static int instr_impl_0xFD(struct cpu *cpu, const struct instr_info *info) {
    fprintf(stderr, "illegal opcode: 0x%02X\n", info->opcode);
    cpu->running = 0;
    return info->cycles;
}

static int instr_impl_0xFE(struct cpu *cpu, const struct instr_info *info) {
    uint8_t v = cpu_fb(cpu);
    DEBUG_INSTR("CP 0x%02X", v);
    alu_cp(cpu, cpu->a, v);
    return info->cycles;
}

static int instr_impl_0xFF(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("RST 38H");
    cpu_push(cpu, cpu->pc);
    cpu->pc = 0x0038;
    return info->cycles;
}

static int instr_impl_prefix_0x00(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("RLC B");
    cpu->b = alu_rlc(cpu, cpu->b);
    return info->cycles;
}

static int instr_impl_prefix_0x01(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("RLC C");
    cpu->c = alu_rlc(cpu, cpu->c);
    return info->cycles;
}

static int instr_impl_prefix_0x02(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("RLC D");
    cpu->d = alu_rlc(cpu, cpu->d);
    return info->cycles;
}

static int instr_impl_prefix_0x03(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("RLC E");
    cpu->e = alu_rlc(cpu, cpu->e);
    return info->cycles;
}

static int instr_impl_prefix_0x04(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("RLC H");
    cpu->h = alu_rlc(cpu, cpu->h);
    return info->cycles;
}

static int instr_impl_prefix_0x05(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("RLC L");
    cpu->l = alu_rlc(cpu, cpu->l);
    return info->cycles;
}

static int instr_impl_prefix_0x06(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("RLC (HL)");
    uint8_t v = IHL_GET();
    mmu_wb(cpu->mmu, cpu->hl, alu_rlc(cpu, v));
    return info->cycles;
}

static int instr_impl_prefix_0x07(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("RLC A");
    cpu->a = alu_rlc(cpu, cpu->a);
    return info->cycles;
}

static int instr_impl_prefix_0x08(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("RRC B");
    cpu->b = alu_rrc(cpu, cpu->b);
    return info->cycles;
}

static int instr_impl_prefix_0x09(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("RRC C");
    cpu->c = alu_rrc(cpu, cpu->c);
    return info->cycles;
}

static int instr_impl_prefix_0x0A(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("RRC D");
    cpu->d = alu_rrc(cpu, cpu->d);
    return info->cycles;
}

static int instr_impl_prefix_0x0B(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("RRC E");
    cpu->e = alu_rrc(cpu, cpu->e);
    return info->cycles;
}

static int instr_impl_prefix_0x0C(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("RRC H");
    cpu->h = alu_rrc(cpu, cpu->h);
    return info->cycles;
}

static int instr_impl_prefix_0x0D(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("RRC L");
    cpu->l = alu_rrc(cpu, cpu->l);
    return info->cycles;
}

static int instr_impl_prefix_0x0E(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("RRC (HL)");
    IHL_SET(alu_rrc(cpu, IHL_GET()));
    return info->cycles;
}

static int instr_impl_prefix_0x0F(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("RRC A");
    cpu->a = alu_rrc(cpu, cpu->a);
    return info->cycles;
}

static int instr_impl_prefix_0x10(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("RL B");
    cpu->b = alu_rl(cpu, cpu->b);
    return info->cycles;
}

static int instr_impl_prefix_0x11(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("RL C");
    cpu->c = alu_rl(cpu, cpu->c);
    return info->cycles;
}

static int instr_impl_prefix_0x12(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("RL D");
    cpu->d = alu_rl(cpu, cpu->d);
    return info->cycles;
}

static int instr_impl_prefix_0x13(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("RL E");
    cpu->e = alu_rl(cpu, cpu->e);
    return info->cycles;
}

static int instr_impl_prefix_0x14(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("RL H");
    cpu->h = alu_rl(cpu, cpu->h);
    return info->cycles;
}

static int instr_impl_prefix_0x15(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("RL L");
    cpu->l = alu_rl(cpu, cpu->l);
    return info->cycles;
}

static int instr_impl_prefix_0x16(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("RL (HL)");
    IHL_SET(alu_rl(cpu, IHL_GET()));
    return info->cycles;
}

static int instr_impl_prefix_0x17(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("RL A");
    cpu->a = alu_rl(cpu, cpu->a);
    return info->cycles;
}

static int instr_impl_prefix_0x18(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("RR B");
    cpu->b = alu_rr(cpu, cpu->b);
    return info->cycles;
}

static int instr_impl_prefix_0x19(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("RR C");
    cpu->c = alu_rr(cpu, cpu->c);
    return info->cycles;
}

static int instr_impl_prefix_0x1A(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("RR D");
    cpu->d = alu_rr(cpu, cpu->d);
    return info->cycles;
}

static int instr_impl_prefix_0x1B(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("RR E");
    cpu->e = alu_rr(cpu, cpu->e);
    return info->cycles;
}

static int instr_impl_prefix_0x1C(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("RR H");
    cpu->h = alu_rr(cpu, cpu->h);
    return info->cycles;
}

static int instr_impl_prefix_0x1D(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("RR L");
    cpu->l = alu_rr(cpu, cpu->l);
    return info->cycles;
}

static int instr_impl_prefix_0x1E(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("RR (HL)");
    IHL_SET(alu_rr(cpu, IHL_GET()));
    return info->cycles;
}

static int instr_impl_prefix_0x1F(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("RR A");
    cpu->a = alu_rr(cpu, cpu->a);
    return info->cycles;
}

static int instr_impl_prefix_0x20(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("SLA B");
    cpu->b = alu_sla(cpu, cpu->b);
    return info->cycles;
}

static int instr_impl_prefix_0x21(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("SLA C");
    cpu->c = alu_sla(cpu, cpu->c);
    return info->cycles;
}

static int instr_impl_prefix_0x22(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("SLA D");
    cpu->d = alu_sla(cpu, cpu->d);
    return info->cycles;
}

static int instr_impl_prefix_0x23(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("SLA E");
    cpu->e = alu_sla(cpu, cpu->e);
    return info->cycles;
}

static int instr_impl_prefix_0x24(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("SLA H");
    cpu->h = alu_sla(cpu, cpu->h);
    return info->cycles;
}

static int instr_impl_prefix_0x25(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("SLA L");
    cpu->l = alu_sla(cpu, cpu->l);
    return info->cycles;
}

static int instr_impl_prefix_0x26(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("SLA (HL)");
    IHL_SET(alu_sla(cpu, IHL_GET()));
    return info->cycles;
}

static int instr_impl_prefix_0x27(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("SLA A");
    cpu->a = alu_sla(cpu, cpu->a);
    return info->cycles;
}

static int instr_impl_prefix_0x28(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("SRA B");
    cpu->b = alu_sra(cpu, cpu->b);
    return info->cycles;
}

static int instr_impl_prefix_0x29(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("SRA C");
    cpu->c = alu_sra(cpu, cpu->c);
    return info->cycles;
}

static int instr_impl_prefix_0x2A(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("SRA D");
    cpu->d = alu_sra(cpu, cpu->d);
    return info->cycles;
}

static int instr_impl_prefix_0x2B(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("SRA E");
    cpu->e = alu_sra(cpu, cpu->e);
    return info->cycles;
}

static int instr_impl_prefix_0x2C(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("SRA H");
    cpu->h = alu_sra(cpu, cpu->h);
    return info->cycles;
}

static int instr_impl_prefix_0x2D(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("SRA L");
    cpu->l = alu_sra(cpu, cpu->l);
    return info->cycles;
}

static int instr_impl_prefix_0x2E(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("SRA (HL)");
    IHL_SET(alu_sra(cpu, IHL_GET()));
    return info->cycles;
}

static int instr_impl_prefix_0x2F(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("SRA A");
    cpu->a = alu_sra(cpu, cpu->a);
    return info->cycles;
}

static int instr_impl_prefix_0x30(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("SWAP B");
    cpu->b = alu_swap(cpu, cpu->b);
    return info->cycles;
}

static int instr_impl_prefix_0x31(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("SWAP C");
    cpu->c = alu_swap(cpu, cpu->c);
    return info->cycles;
}

static int instr_impl_prefix_0x32(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("SWAP D");
    cpu->d = alu_swap(cpu, cpu->d);
    return info->cycles;
}

static int instr_impl_prefix_0x33(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("SWAP E");
    cpu->e = alu_swap(cpu, cpu->e);
    return info->cycles;
}

static int instr_impl_prefix_0x34(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("SWAP H");
    cpu->h = alu_swap(cpu, cpu->h);
    return info->cycles;
}

static int instr_impl_prefix_0x35(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("SWAP L");
    cpu->l = alu_swap(cpu, cpu->l);
    return info->cycles;
}

static int instr_impl_prefix_0x36(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("SWAP (HL)");
    IHL_SET(alu_swap(cpu, IHL_GET()));
    return info->cycles;
}

static int instr_impl_prefix_0x37(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("SWAP A");
    cpu->a = alu_swap(cpu, cpu->a);
    return info->cycles;
}

static int instr_impl_prefix_0x38(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("SRL B");
    cpu->b = alu_srl(cpu, cpu->b);
    return info->cycles;
}

static int instr_impl_prefix_0x39(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("SRL C");
    cpu->c = alu_srl(cpu, cpu->c);
    return info->cycles;
}

static int instr_impl_prefix_0x3A(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("SRL D");
    cpu->d = alu_srl(cpu, cpu->d);
    return info->cycles;
}

static int instr_impl_prefix_0x3B(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("SRL E");
    cpu->e = alu_srl(cpu, cpu->e);
    return info->cycles;
}

static int instr_impl_prefix_0x3C(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("SRL H");
    cpu->h = alu_srl(cpu, cpu->h);
    return info->cycles;
}

static int instr_impl_prefix_0x3D(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("SRL L");
    cpu->l = alu_srl(cpu, cpu->l);
    return info->cycles;
}

static int instr_impl_prefix_0x3E(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("SRL (HL)");
    IHL_SET(alu_srl(cpu, IHL_GET()));
    return info->cycles;
}

static int instr_impl_prefix_0x3F(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("SRL A");
    cpu->a = alu_srl(cpu, cpu->a);
    return info->cycles;
}

static int instr_impl_prefix_0x40(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("BIT 0,B");
    alu_bit(cpu, cpu->b, 0);
    return info->cycles;
}

static int instr_impl_prefix_0x41(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("BIT 0,C");
    alu_bit(cpu, cpu->c, 0);
    return info->cycles;
}

static int instr_impl_prefix_0x42(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("BIT 0,D");
    alu_bit(cpu, cpu->d, 0);
    return info->cycles;
}

static int instr_impl_prefix_0x43(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("BIT 0,E");
    alu_bit(cpu, cpu->e, 0);
    return info->cycles;
}

static int instr_impl_prefix_0x44(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("BIT 0,H");
    alu_bit(cpu, cpu->h, 0);
    return info->cycles;
}

static int instr_impl_prefix_0x45(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("BIT 0,L");
    alu_bit(cpu, cpu->l, 0);
    return info->cycles;
}

static int instr_impl_prefix_0x46(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("BIT 0,(HL)");
    alu_bit(cpu, IHL_GET(), 0);
    return info->cycles;
}

static int instr_impl_prefix_0x47(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("BIT 0,A");
    alu_bit(cpu, cpu->a, 0);
    return info->cycles;
}

static int instr_impl_prefix_0x48(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("BIT 1,B");
    alu_bit(cpu, cpu->b, 1);
    return info->cycles;
}

static int instr_impl_prefix_0x49(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("BIT 1,C");
    alu_bit(cpu, cpu->c, 1);
    return info->cycles;
}

static int instr_impl_prefix_0x4A(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("BIT 1,D");
    alu_bit(cpu, cpu->d, 1);
    return info->cycles;
}

static int instr_impl_prefix_0x4B(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("BIT 1,E");
    alu_bit(cpu, cpu->e, 1);
    return info->cycles;
}

static int instr_impl_prefix_0x4C(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("BIT 1,H");
    alu_bit(cpu, cpu->h, 1);
    return info->cycles;
}

static int instr_impl_prefix_0x4D(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("BIT 1,L");
    alu_bit(cpu, cpu->l, 1);
    return info->cycles;
}

static int instr_impl_prefix_0x4E(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("BIT 1,(HL)");
    alu_bit(cpu, IHL_GET(), 1);
    return info->cycles;
}

static int instr_impl_prefix_0x4F(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("BIT 1,A");
    alu_bit(cpu, cpu->a, 1);
    return info->cycles;
}

static int instr_impl_prefix_0x50(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("BIT 2,B");
    alu_bit(cpu, cpu->b, 2);
    return info->cycles;
}

static int instr_impl_prefix_0x51(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("BIT 2,C");
    alu_bit(cpu, cpu->c, 2);
    return info->cycles;
}

static int instr_impl_prefix_0x52(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("BIT 2,D");
    alu_bit(cpu, cpu->d, 2);
    return info->cycles;
}

static int instr_impl_prefix_0x53(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("BIT 2,E");
    alu_bit(cpu, cpu->e, 2);
    return info->cycles;
}

static int instr_impl_prefix_0x54(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("BIT 2,H");
    alu_bit(cpu, cpu->h, 2);
    return info->cycles;
}

static int instr_impl_prefix_0x55(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("BIT 2,L");
    alu_bit(cpu, cpu->l, 2);
    return info->cycles;
}

static int instr_impl_prefix_0x56(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("BIT 2,(HL)");
    alu_bit(cpu, IHL_GET(), 2);
    return info->cycles;
}

static int instr_impl_prefix_0x57(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("BIT 2,A");
    alu_bit(cpu, cpu->a, 2);
    return info->cycles;
}

static int instr_impl_prefix_0x58(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("BIT 3,B");
    alu_bit(cpu, cpu->b, 3);
    return info->cycles;
}

static int instr_impl_prefix_0x59(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("BIT 3,C");
    alu_bit(cpu, cpu->c, 3);
    return info->cycles;
}

static int instr_impl_prefix_0x5A(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("BIT 3,D");
    alu_bit(cpu, cpu->d, 3);
    return info->cycles;
}

static int instr_impl_prefix_0x5B(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("BIT 3,E");
    alu_bit(cpu, cpu->e, 3);
    return info->cycles;
}

static int instr_impl_prefix_0x5C(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("BIT 3,H");
    alu_bit(cpu, cpu->h, 3);
    return info->cycles;
}

static int instr_impl_prefix_0x5D(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("BIT 3,L");
    alu_bit(cpu, cpu->l, 3);
    return info->cycles;
}

static int instr_impl_prefix_0x5E(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("BIT 3,(HL)");
    alu_bit(cpu, IHL_GET(), 3);
    return info->cycles;
}

static int instr_impl_prefix_0x5F(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("BIT 3,A");
    alu_bit(cpu, cpu->a, 3);
    return info->cycles;
}

static int instr_impl_prefix_0x60(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("BIT 4,B");
    alu_bit(cpu, cpu->b, 4);
    return info->cycles;
}

static int instr_impl_prefix_0x61(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("BIT 4,C");
    alu_bit(cpu, cpu->c, 4);
    return info->cycles;
}

static int instr_impl_prefix_0x62(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("BIT 4,D");
    alu_bit(cpu, cpu->d, 4);
    return info->cycles;
}

static int instr_impl_prefix_0x63(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("BIT 4,E");
    alu_bit(cpu, cpu->e, 4);
    return info->cycles;
}

static int instr_impl_prefix_0x64(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("BIT 4,H");
    alu_bit(cpu, cpu->h, 4);
    return info->cycles;
}

static int instr_impl_prefix_0x65(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("BIT 4,L");
    alu_bit(cpu, cpu->l, 4);
    return info->cycles;
}

static int instr_impl_prefix_0x66(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("BIT 4,(HL)");
    alu_bit(cpu, IHL_GET(), 4);
    return info->cycles;
}

static int instr_impl_prefix_0x67(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("BIT 4,A");
    alu_bit(cpu, cpu->a, 4);
    return info->cycles;
}

static int instr_impl_prefix_0x68(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("BIT 5,B");
    alu_bit(cpu, cpu->b, 5);
    return info->cycles;
}

static int instr_impl_prefix_0x69(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("BIT 5,C");
    alu_bit(cpu, cpu->c, 5);
    return info->cycles;
}

static int instr_impl_prefix_0x6A(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("BIT 5,D");
    alu_bit(cpu, cpu->d, 5);
    return info->cycles;
}

static int instr_impl_prefix_0x6B(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("BIT 5,E");
    alu_bit(cpu, cpu->e, 5);
    return info->cycles;
}

static int instr_impl_prefix_0x6C(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("BIT 5,H");
    alu_bit(cpu, cpu->h, 5);
    return info->cycles;
}

static int instr_impl_prefix_0x6D(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("BIT 5,L");
    alu_bit(cpu, cpu->l, 5);
    return info->cycles;
}

static int instr_impl_prefix_0x6E(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("BIT 5,(HL)");
    alu_bit(cpu, IHL_GET(), 5);
    return info->cycles;
}

static int instr_impl_prefix_0x6F(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("BIT 5,A");
    alu_bit(cpu, cpu->a, 5);
    return info->cycles;
}

static int instr_impl_prefix_0x70(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("BIT 6,B");
    alu_bit(cpu, cpu->b, 6);
    return info->cycles;
}

static int instr_impl_prefix_0x71(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("BIT 6,C");
    alu_bit(cpu, cpu->c, 6);
    return info->cycles;
}

static int instr_impl_prefix_0x72(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("BIT 6,D");
    alu_bit(cpu, cpu->d, 6);
    return info->cycles;
}

static int instr_impl_prefix_0x73(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("BIT 6,E");
    alu_bit(cpu, cpu->e, 6);
    return info->cycles;
}

static int instr_impl_prefix_0x74(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("BIT 6,H");
    alu_bit(cpu, cpu->h, 6);
    return info->cycles;
}

static int instr_impl_prefix_0x75(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("BIT 6,L");
    alu_bit(cpu, cpu->l, 6);
    return info->cycles;
}

static int instr_impl_prefix_0x76(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("BIT 6,(HL)");
    alu_bit(cpu, IHL_GET(), 6);
    return info->cycles;
}

static int instr_impl_prefix_0x77(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("BIT 6,A");
    alu_bit(cpu, cpu->a, 6);
    return info->cycles;
}

static int instr_impl_prefix_0x78(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("BIT 7,B");
    alu_bit(cpu, cpu->b, 7);
    return info->cycles;
}

static int instr_impl_prefix_0x79(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("BIT 7,C");
    alu_bit(cpu, cpu->c, 7);
    return info->cycles;
}

static int instr_impl_prefix_0x7A(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("BIT 7,D");
    alu_bit(cpu, cpu->d, 7);
    return info->cycles;
}

static int instr_impl_prefix_0x7B(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("BIT 7,E");
    alu_bit(cpu, cpu->e, 7);
    return info->cycles;
}

static int instr_impl_prefix_0x7C(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("BIT 7,H");
    alu_bit(cpu, cpu->h, 7);
    return info->cycles;
}

static int instr_impl_prefix_0x7D(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("BIT 7,L");
    alu_bit(cpu, cpu->l, 7);
    return info->cycles;
}

static int instr_impl_prefix_0x7E(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("BIT 7,(HL)");
    alu_bit(cpu, IHL_GET(), 7);
    return info->cycles;
}

static int instr_impl_prefix_0x7F(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("BIT 7,A");
    alu_bit(cpu, cpu->a, 7);
    return info->cycles;
}

static int instr_impl_prefix_0x80(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("RES 0,B");
    cpu->b = alu_res(cpu, cpu->b, 0);
    return info->cycles;
}

static int instr_impl_prefix_0x81(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("RES 0,C");
    cpu->c = alu_res(cpu, cpu->c, 0);
    return info->cycles;
}

static int instr_impl_prefix_0x82(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("RES 0,D");
    cpu->d = alu_res(cpu, cpu->d, 0);
    return info->cycles;
}

static int instr_impl_prefix_0x83(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("RES 0,E");
    cpu->e = alu_res(cpu, cpu->e, 0);
    return info->cycles;
}

static int instr_impl_prefix_0x84(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("RES 0,H");
    cpu->h = alu_res(cpu, cpu->h, 0);
    return info->cycles;
}

static int instr_impl_prefix_0x85(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("RES 0,L");
    cpu->l = alu_res(cpu, cpu->l, 0);
    return info->cycles;
}

static int instr_impl_prefix_0x86(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("RES 0,(HL)");
    IHL_SET(alu_res(cpu, IHL_GET(), 0));
    return info->cycles;
}

static int instr_impl_prefix_0x87(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("RES 0,A");
    cpu->a = alu_res(cpu, cpu->a, 0);
    return info->cycles;
}

static int instr_impl_prefix_0x88(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("RES 1,B");
    cpu->b = alu_res(cpu, cpu->b, 1);
    return info->cycles;
}

static int instr_impl_prefix_0x89(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("RES 1,C");
    cpu->c = alu_res(cpu, cpu->c, 1);
    return info->cycles;
}

static int instr_impl_prefix_0x8A(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("RES 1,D");
    cpu->d = alu_res(cpu, cpu->d, 1);
    return info->cycles;
}

static int instr_impl_prefix_0x8B(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("RES 1,E");
    cpu->e = alu_res(cpu, cpu->e, 1);
    return info->cycles;
}

static int instr_impl_prefix_0x8C(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("RES 1,H");
    cpu->h = alu_res(cpu, cpu->h, 1);
    return info->cycles;
}

static int instr_impl_prefix_0x8D(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("RES 1,L");
    cpu->l = alu_res(cpu, cpu->l, 1);
    return info->cycles;
}

static int instr_impl_prefix_0x8E(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("RES 1,(HL)");
    IHL_SET(alu_res(cpu, IHL_GET(), 1));
    return info->cycles;
}

static int instr_impl_prefix_0x8F(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("RES 1,A");
    cpu->a = alu_res(cpu, cpu->a, 1);
    return info->cycles;
}

static int instr_impl_prefix_0x90(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("RES 2,B");
    cpu->b = alu_res(cpu, cpu->b, 2);
    return info->cycles;
}

static int instr_impl_prefix_0x91(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("RES 2,C");
    cpu->c = alu_res(cpu, cpu->c, 2);
    return info->cycles;
}

static int instr_impl_prefix_0x92(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("RES 2,D");
    cpu->d = alu_res(cpu, cpu->d, 2);
    return info->cycles;
}

static int instr_impl_prefix_0x93(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("RES 2,E");
    cpu->e = alu_res(cpu, cpu->e, 2);
    return info->cycles;
}

static int instr_impl_prefix_0x94(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("RES 2,H");
    cpu->h = alu_res(cpu, cpu->h, 2);
    return info->cycles;
}

static int instr_impl_prefix_0x95(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("RES 2,L");
    cpu->l = alu_res(cpu, cpu->l, 2);
    return info->cycles;
}

static int instr_impl_prefix_0x96(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("RES 2,(HL)");
    IHL_SET(alu_res(cpu, IHL_GET(), 2));
    return info->cycles;
}

static int instr_impl_prefix_0x97(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("RES 2,A");
    cpu->a = alu_res(cpu, cpu->a, 2);
    return info->cycles;
}

static int instr_impl_prefix_0x98(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("RES 3,B");
    cpu->b = alu_res(cpu, cpu->b, 3);
    return info->cycles;
}

static int instr_impl_prefix_0x99(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("RES 3,C");
    cpu->c = alu_res(cpu, cpu->c, 3);
    return info->cycles;
}

static int instr_impl_prefix_0x9A(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("RES 3,D");
    cpu->d = alu_res(cpu, cpu->d, 3);
    return info->cycles;
}

static int instr_impl_prefix_0x9B(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("RES 3,E");
    cpu->e = alu_res(cpu, cpu->e, 3);
    return info->cycles;
}

static int instr_impl_prefix_0x9C(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("RES 3,H");
    cpu->h = alu_res(cpu, cpu->h, 3);
    return info->cycles;
}

static int instr_impl_prefix_0x9D(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("RES 3,L");
    cpu->l = alu_res(cpu, cpu->l, 3);
    return info->cycles;
}

static int instr_impl_prefix_0x9E(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("RES 3,(HL)");
    IHL_SET(alu_res(cpu, IHL_GET(), 3));
    return info->cycles;
}

static int instr_impl_prefix_0x9F(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("RES 3,A");
    cpu->a = alu_res(cpu, cpu->a, 3);
    return info->cycles;
}

static int instr_impl_prefix_0xA0(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("RES 4,B");
    cpu->b = alu_res(cpu, cpu->b, 4);
    return info->cycles;
}

static int instr_impl_prefix_0xA1(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("RES 4,C");
    cpu->c = alu_res(cpu, cpu->c, 4);
    return info->cycles;
}

static int instr_impl_prefix_0xA2(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("RES 4,D");
    cpu->d = alu_res(cpu, cpu->d, 4);
    return info->cycles;
}

static int instr_impl_prefix_0xA3(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("RES 4,E");
    cpu->e = alu_res(cpu, cpu->e, 4);
    return info->cycles;
}

static int instr_impl_prefix_0xA4(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("RES 4,H");
    cpu->h = alu_res(cpu, cpu->h, 4);
    return info->cycles;
}

static int instr_impl_prefix_0xA5(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("RES 4,L");
    cpu->l = alu_res(cpu, cpu->l, 4);
    return info->cycles;
}

static int instr_impl_prefix_0xA6(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("RES 4,(HL)");
    IHL_SET(alu_res(cpu, IHL_GET(), 4));
    return info->cycles;
}

static int instr_impl_prefix_0xA7(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("RES 4,A");
    cpu->a = alu_res(cpu, cpu->a, 4);
    return info->cycles;
}

static int instr_impl_prefix_0xA8(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("RES 5,B");
    cpu->b = alu_res(cpu, cpu->b, 5);
    return info->cycles;
}

static int instr_impl_prefix_0xA9(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("RES 5,C");
    cpu->c = alu_res(cpu, cpu->c, 5);
    return info->cycles;
}

static int instr_impl_prefix_0xAA(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("RES 5,D");
    cpu->d = alu_res(cpu, cpu->d, 5);
    return info->cycles;
}

static int instr_impl_prefix_0xAB(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("RES 5,E");
    cpu->e = alu_res(cpu, cpu->e, 5);
    return info->cycles;
}

static int instr_impl_prefix_0xAC(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("RES 5,H");
    cpu->h = alu_res(cpu, cpu->h, 5);
    return info->cycles;
}

static int instr_impl_prefix_0xAD(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("RES 5,L");
    cpu->l = alu_res(cpu, cpu->l, 5);
    return info->cycles;
}

static int instr_impl_prefix_0xAE(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("RES 5,(HL)");
    IHL_SET(alu_res(cpu, IHL_GET(), 5));
    return info->cycles;
}

static int instr_impl_prefix_0xAF(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("RES 5,A");
    cpu->a = alu_res(cpu, cpu->a, 5);
    return info->cycles;
}

static int instr_impl_prefix_0xB0(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("RES 6,B");
    cpu->b = alu_res(cpu, cpu->b, 6);
    return info->cycles;
}

static int instr_impl_prefix_0xB1(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("RES 6,C");
    cpu->c = alu_res(cpu, cpu->c, 6);
    return info->cycles;
}

static int instr_impl_prefix_0xB2(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("RES 6,D");
    cpu->d = alu_res(cpu, cpu->d, 6);
    return info->cycles;
}

static int instr_impl_prefix_0xB3(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("RES 6,E");
    cpu->e = alu_res(cpu, cpu->e, 6);
    return info->cycles;
}

static int instr_impl_prefix_0xB4(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("RES 6,H");
    cpu->h = alu_res(cpu, cpu->h, 6);
    return info->cycles;
}

static int instr_impl_prefix_0xB5(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("RES 6,L");
    cpu->l = alu_res(cpu, cpu->l, 6);
    return info->cycles;
}

static int instr_impl_prefix_0xB6(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("RES 6,(HL)");
    IHL_SET(alu_res(cpu, IHL_GET(), 6));
    return info->cycles;
}

static int instr_impl_prefix_0xB7(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("RES 6,A");
    cpu->a = alu_res(cpu, cpu->a, 6);
    return info->cycles;
}

static int instr_impl_prefix_0xB8(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("RES 7,B");
    cpu->b = alu_res(cpu, cpu->b, 7);
    return info->cycles;
}

static int instr_impl_prefix_0xB9(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("RES 7,C");
    cpu->c = alu_res(cpu, cpu->c, 7);
    return info->cycles;
}

static int instr_impl_prefix_0xBA(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("RES 7,D");
    cpu->d = alu_res(cpu, cpu->d, 7);
    return info->cycles;
}

static int instr_impl_prefix_0xBB(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("RES 7,E");
    cpu->e = alu_res(cpu, cpu->e, 7);
    return info->cycles;
}

static int instr_impl_prefix_0xBC(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("RES 7,H");
    cpu->h = alu_res(cpu, cpu->h, 7);
    return info->cycles;
}

static int instr_impl_prefix_0xBD(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("RES 7,L");
    cpu->l = alu_res(cpu, cpu->l, 7);
    return info->cycles;
}

static int instr_impl_prefix_0xBE(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("RES 7,(HL)");
    uint8_t v = IHL_GET();
    mmu_wb(cpu->mmu, cpu->hl, alu_res(cpu, v, 7));
    return info->cycles;
}

static int instr_impl_prefix_0xBF(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("RES 7,A");
    cpu->a = alu_res(cpu, cpu->a, 7);
    return info->cycles;
}

static int instr_impl_prefix_0xC0(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("SET 0,B");
    cpu->b = alu_set(cpu, cpu->b, 0);
    return info->cycles;
}

static int instr_impl_prefix_0xC1(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("SET 0,C");
    cpu->c = alu_set(cpu, cpu->c, 0);
    return info->cycles;
}

static int instr_impl_prefix_0xC2(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("SET 0,D");
    cpu->d = alu_set(cpu, cpu->d, 0);
    return info->cycles;
}

static int instr_impl_prefix_0xC3(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("SET 0,E");
    cpu->e = alu_set(cpu, cpu->e, 0);
    return info->cycles;
}

static int instr_impl_prefix_0xC4(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("SET 0,H");
    cpu->h = alu_set(cpu, cpu->h, 0);
    return info->cycles;
}

static int instr_impl_prefix_0xC5(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("SET 0,L");
    cpu->l = alu_set(cpu, cpu->l, 0);
    return info->cycles;
}

static int instr_impl_prefix_0xC6(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("SET 0,(HL)");
    IHL_SET(alu_set(cpu, IHL_GET(), 0));
    return info->cycles;
}

static int instr_impl_prefix_0xC7(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("SET 0,A");
    cpu->a = alu_set(cpu, cpu->a, 0);
    return info->cycles;
}

static int instr_impl_prefix_0xC8(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("SET 1,B");
    cpu->b = alu_set(cpu, cpu->b, 1);
    return info->cycles;
}

static int instr_impl_prefix_0xC9(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("SET 1,C");
    cpu->c = alu_set(cpu, cpu->c, 1);
    return info->cycles;
}

static int instr_impl_prefix_0xCA(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("SET 1,D");
    cpu->d = alu_set(cpu, cpu->d, 1);
    return info->cycles;
}

static int instr_impl_prefix_0xCB(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("SET 1,E");
    cpu->e = alu_set(cpu, cpu->e, 1);
    return info->cycles;
}

static int instr_impl_prefix_0xCC(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("SET 1,H");
    cpu->h = alu_set(cpu, cpu->h, 1);
    return info->cycles;
}

static int instr_impl_prefix_0xCD(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("SET 1,L");
    cpu->l = alu_set(cpu, cpu->l, 1);
    return info->cycles;
}

static int instr_impl_prefix_0xCE(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("SET 1,(HL)");
    uint8_t v = IHL_GET();
    mmu_wb(cpu->mmu, cpu->hl, alu_set(cpu, v, 1));
    return info->cycles;
}

static int instr_impl_prefix_0xCF(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("SET 1,A");
    cpu->a = alu_set(cpu, cpu->a, 1);
    return info->cycles;
}

static int instr_impl_prefix_0xD0(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("SET 2,B");
    cpu->b = alu_set(cpu, cpu->b, 2);
    return info->cycles;
}

static int instr_impl_prefix_0xD1(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("SET 2,C");
    cpu->c = alu_set(cpu, cpu->c, 2);
    return info->cycles;
}

static int instr_impl_prefix_0xD2(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("SET 2,D");
    cpu->d = alu_set(cpu, cpu->d, 2);
    return info->cycles;
}

static int instr_impl_prefix_0xD3(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("SET 2,E");
    cpu->e = alu_set(cpu, cpu->e, 2);
    return info->cycles;
}

static int instr_impl_prefix_0xD4(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("SET 2,H");
    cpu->h = alu_set(cpu, cpu->h, 2);
    return info->cycles;
}

static int instr_impl_prefix_0xD5(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("SET 2,L");
    cpu->l = alu_set(cpu, cpu->l, 2);
    return info->cycles;
}

static int instr_impl_prefix_0xD6(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("SET 2,(HL)");
    uint8_t v = IHL_GET();
    mmu_wb(cpu->mmu, cpu->hl, alu_set(cpu, v, 2));
    return info->cycles;
}

static int instr_impl_prefix_0xD7(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("SET 2,A");
    cpu->a = alu_set(cpu, cpu->a, 2);
    return info->cycles;
}

static int instr_impl_prefix_0xD8(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("SET 3,B");
    cpu->b = alu_set(cpu, cpu->b, 3);
    return info->cycles;
}

static int instr_impl_prefix_0xD9(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("SET 3,C");
    cpu->c = alu_set(cpu, cpu->c, 3);
    return info->cycles;
}

static int instr_impl_prefix_0xDA(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("SET 3,D");
    cpu->d = alu_set(cpu, cpu->d, 3);
    return info->cycles;
}

static int instr_impl_prefix_0xDB(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("SET 3,E");
    cpu->e = alu_set(cpu, cpu->e, 3);
    return info->cycles;
}

static int instr_impl_prefix_0xDC(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("SET 3,H");
    cpu->h = alu_set(cpu, cpu->h, 3);
    return info->cycles;
}

static int instr_impl_prefix_0xDD(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("SET 3,L");
    cpu->l = alu_set(cpu, cpu->l, 3);
    return info->cycles;
}

static int instr_impl_prefix_0xDE(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("SET 3,(HL)");
    uint8_t v = IHL_GET();
    mmu_wb(cpu->mmu, cpu->hl, alu_set(cpu, v, 3));
    return info->cycles;
}

static int instr_impl_prefix_0xDF(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("SET 3,A");
    cpu->a = alu_set(cpu, cpu->a, 3);
    return info->cycles;
}

static int instr_impl_prefix_0xE0(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("SET 4,B");
    cpu->b = alu_set(cpu, cpu->b, 4);
    return info->cycles;
}

static int instr_impl_prefix_0xE1(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("SET 4,C");
    cpu->c = alu_set(cpu, cpu->c, 4);
    return info->cycles;
}

static int instr_impl_prefix_0xE2(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("SET 4,D");
    cpu->d = alu_set(cpu, cpu->d, 4);
    return info->cycles;
}

static int instr_impl_prefix_0xE3(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("SET 4,E");
    cpu->e = alu_set(cpu, cpu->e, 4);
    return info->cycles;
}

static int instr_impl_prefix_0xE4(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("SET 4,H");
    cpu->h = alu_set(cpu, cpu->h, 4);
    return info->cycles;
}

static int instr_impl_prefix_0xE5(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("SET 4,L");
    cpu->l = alu_set(cpu, cpu->l, 4);
    return info->cycles;
}

static int instr_impl_prefix_0xE6(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("SET 4,(HL)");
    uint8_t v = IHL_GET();
    mmu_wb(cpu->mmu, cpu->hl, alu_set(cpu, v, 4));
    return info->cycles;
}

static int instr_impl_prefix_0xE7(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("SET 4,A");
    cpu->a = alu_set(cpu, cpu->a, 4);
    return info->cycles;
}

static int instr_impl_prefix_0xE8(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("SET 5,B");
    cpu->b = alu_set(cpu, cpu->b, 5);
    return info->cycles;
}

static int instr_impl_prefix_0xE9(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("SET 5,C");
    cpu->c = alu_set(cpu, cpu->c, 5);
    return info->cycles;
}

static int instr_impl_prefix_0xEA(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("SET 5,D");
    cpu->d = alu_set(cpu, cpu->d, 5);
    return info->cycles;
}

static int instr_impl_prefix_0xEB(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("SET 5,E");
    cpu->e = alu_set(cpu, cpu->e, 5);
    return info->cycles;
}

static int instr_impl_prefix_0xEC(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("SET 5,H");
    cpu->h = alu_set(cpu, cpu->h, 5);
    return info->cycles;
}

static int instr_impl_prefix_0xED(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("SET 5,L");
    cpu->l = alu_set(cpu, cpu->l, 5);
    return info->cycles;
}

static int instr_impl_prefix_0xEE(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("SET 5,(HL)");
    IHL_SET(alu_set(cpu, IHL_GET(), 5));
    return info->cycles;
}

static int instr_impl_prefix_0xEF(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("SET 5,A");
    cpu->a = alu_set(cpu, cpu->a, 5);
    return info->cycles;
}

static int instr_impl_prefix_0xF0(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("SET 6,B");
    cpu->b = alu_set(cpu, cpu->b, 6);
    return info->cycles;
}

static int instr_impl_prefix_0xF1(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("SET 6,C");
    cpu->c = alu_set(cpu, cpu->c, 6);
    return info->cycles;
}

static int instr_impl_prefix_0xF2(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("SET 6,D");
    cpu->d = alu_set(cpu, cpu->d, 6);
    return info->cycles;
}

static int instr_impl_prefix_0xF3(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("SET 6,E");
    cpu->e = alu_set(cpu, cpu->e, 6);
    return info->cycles;
}

static int instr_impl_prefix_0xF4(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("SET 6,H");
    cpu->h = alu_set(cpu, cpu->h, 6);
    return info->cycles;
}

static int instr_impl_prefix_0xF5(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("SET 6,L");
    cpu->l = alu_set(cpu, cpu->l, 6);
    return info->cycles;
}

static int instr_impl_prefix_0xF6(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("SET 6,(HL)");
    IHL_SET(alu_set(cpu, IHL_GET(), 6));
    return info->cycles;
}

static int instr_impl_prefix_0xF7(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("SET 6,A");
    cpu->a = alu_set(cpu, cpu->a, 6);
    return info->cycles;
}

static int instr_impl_prefix_0xF8(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("SET 7,B");
    cpu->b = alu_set(cpu, cpu->b, 7);
    return info->cycles;
}

static int instr_impl_prefix_0xF9(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("SET 7,C");
    cpu->c = alu_set(cpu, cpu->c, 7);
    return info->cycles;
}

static int instr_impl_prefix_0xFA(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("SET 7,D");
    cpu->d = alu_set(cpu, cpu->d, 7);
    return info->cycles;
}

static int instr_impl_prefix_0xFB(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("SET 7,E");
    cpu->e = alu_set(cpu, cpu->e, 7);
    return info->cycles;
}

static int instr_impl_prefix_0xFC(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("SET 7,H");
    cpu->h = alu_set(cpu, cpu->h, 7);
    return info->cycles;
}

static int instr_impl_prefix_0xFD(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("SET 7,L");
    cpu->l = alu_set(cpu, cpu->l, 7);
    return info->cycles;
}

static int instr_impl_prefix_0xFE(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("SET 7,(HL)");
    uint8_t v = IHL_GET();
    mmu_wb(cpu->mmu, cpu->hl, alu_set(cpu, v, 7));
    return info->cycles;
}

static int instr_impl_prefix_0xFF(struct cpu *cpu, const struct instr_info *info) {
    DEBUG_INSTR("SET 7,A");
    cpu->a = alu_set(cpu, cpu->a, 7);
    return info->cycles;
}

const instr_impl_fn INSTR_IMPL[256] = {
    instr_impl_0x00,
    instr_impl_0x01,
    instr_impl_0x02,
    instr_impl_0x03,
    instr_impl_0x04,
    instr_impl_0x05,
    instr_impl_0x06,
    instr_impl_0x07,
    instr_impl_0x08,
    instr_impl_0x09,
    instr_impl_0x0A,
    instr_impl_0x0B,
    instr_impl_0x0C,
    instr_impl_0x0D,
    instr_impl_0x0E,
    instr_impl_0x0F,
    instr_impl_0x10,
    instr_impl_0x11,
    instr_impl_0x12,
    instr_impl_0x13,
    instr_impl_0x14,
    instr_impl_0x15,
    instr_impl_0x16,
    instr_impl_0x17,
    instr_impl_0x18,
    instr_impl_0x19,
    instr_impl_0x1A,
    instr_impl_0x1B,
    instr_impl_0x1C,
    instr_impl_0x1D,
    instr_impl_0x1E,
    instr_impl_0x1F,
    instr_impl_0x20,
    instr_impl_0x21,
    instr_impl_0x22,
    instr_impl_0x23,
    instr_impl_0x24,
    instr_impl_0x25,
    instr_impl_0x26,
    instr_impl_0x27,
    instr_impl_0x28,
    instr_impl_0x29,
    instr_impl_0x2A,
    instr_impl_0x2B,
    instr_impl_0x2C,
    instr_impl_0x2D,
    instr_impl_0x2E,
    instr_impl_0x2F,
    instr_impl_0x30,
    instr_impl_0x31,
    instr_impl_0x32,
    instr_impl_0x33,
    instr_impl_0x34,
    instr_impl_0x35,
    instr_impl_0x36,
    instr_impl_0x37,
    instr_impl_0x38,
    instr_impl_0x39,
    instr_impl_0x3A,
    instr_impl_0x3B,
    instr_impl_0x3C,
    instr_impl_0x3D,
    instr_impl_0x3E,
    instr_impl_0x3F,
    instr_impl_0x40,
    instr_impl_0x41,
    instr_impl_0x42,
    instr_impl_0x43,
    instr_impl_0x44,
    instr_impl_0x45,
    instr_impl_0x46,
    instr_impl_0x47,
    instr_impl_0x48,
    instr_impl_0x49,
    instr_impl_0x4A,
    instr_impl_0x4B,
    instr_impl_0x4C,
    instr_impl_0x4D,
    instr_impl_0x4E,
    instr_impl_0x4F,
    instr_impl_0x50,
    instr_impl_0x51,
    instr_impl_0x52,
    instr_impl_0x53,
    instr_impl_0x54,
    instr_impl_0x55,
    instr_impl_0x56,
    instr_impl_0x57,
    instr_impl_0x58,
    instr_impl_0x59,
    instr_impl_0x5A,
    instr_impl_0x5B,
    instr_impl_0x5C,
    instr_impl_0x5D,
    instr_impl_0x5E,
    instr_impl_0x5F,
    instr_impl_0x60,
    instr_impl_0x61,
    instr_impl_0x62,
    instr_impl_0x63,
    instr_impl_0x64,
    instr_impl_0x65,
    instr_impl_0x66,
    instr_impl_0x67,
    instr_impl_0x68,
    instr_impl_0x69,
    instr_impl_0x6A,
    instr_impl_0x6B,
    instr_impl_0x6C,
    instr_impl_0x6D,
    instr_impl_0x6E,
    instr_impl_0x6F,
    instr_impl_0x70,
    instr_impl_0x71,
    instr_impl_0x72,
    instr_impl_0x73,
    instr_impl_0x74,
    instr_impl_0x75,
    instr_impl_0x76,
    instr_impl_0x77,
    instr_impl_0x78,
    instr_impl_0x79,
    instr_impl_0x7A,
    instr_impl_0x7B,
    instr_impl_0x7C,
    instr_impl_0x7D,
    instr_impl_0x7E,
    instr_impl_0x7F,
    instr_impl_0x80,
    instr_impl_0x81,
    instr_impl_0x82,
    instr_impl_0x83,
    instr_impl_0x84,
    instr_impl_0x85,
    instr_impl_0x86,
    instr_impl_0x87,
    instr_impl_0x88,
    instr_impl_0x89,
    instr_impl_0x8A,
    instr_impl_0x8B,
    instr_impl_0x8C,
    instr_impl_0x8D,
    instr_impl_0x8E,
    instr_impl_0x8F,
    instr_impl_0x90,
    instr_impl_0x91,
    instr_impl_0x92,
    instr_impl_0x93,
    instr_impl_0x94,
    instr_impl_0x95,
    instr_impl_0x96,
    instr_impl_0x97,
    instr_impl_0x98,
    instr_impl_0x99,
    instr_impl_0x9A,
    instr_impl_0x9B,
    instr_impl_0x9C,
    instr_impl_0x9D,
    instr_impl_0x9E,
    instr_impl_0x9F,
    instr_impl_0xA0,
    instr_impl_0xA1,
    instr_impl_0xA2,
    instr_impl_0xA3,
    instr_impl_0xA4,
    instr_impl_0xA5,
    instr_impl_0xA6,
    instr_impl_0xA7,
    instr_impl_0xA8,
    instr_impl_0xA9,
    instr_impl_0xAA,
    instr_impl_0xAB,
    instr_impl_0xAC,
    instr_impl_0xAD,
    instr_impl_0xAE,
    instr_impl_0xAF,
    instr_impl_0xB0,
    instr_impl_0xB1,
    instr_impl_0xB2,
    instr_impl_0xB3,
    instr_impl_0xB4,
    instr_impl_0xB5,
    instr_impl_0xB6,
    instr_impl_0xB7,
    instr_impl_0xB8,
    instr_impl_0xB9,
    instr_impl_0xBA,
    instr_impl_0xBB,
    instr_impl_0xBC,
    instr_impl_0xBD,
    instr_impl_0xBE,
    instr_impl_0xBF,
    instr_impl_0xC0,
    instr_impl_0xC1,
    instr_impl_0xC2,
    instr_impl_0xC3,
    instr_impl_0xC4,
    instr_impl_0xC5,
    instr_impl_0xC6,
    instr_impl_0xC7,
    instr_impl_0xC8,
    instr_impl_0xC9,
    instr_impl_0xCA,
    instr_impl_0xCB,
    instr_impl_0xCC,
    instr_impl_0xCD,
    instr_impl_0xCE,
    instr_impl_0xCF,
    instr_impl_0xD0,
    instr_impl_0xD1,
    instr_impl_0xD2,
    instr_impl_0xD3,
    instr_impl_0xD4,
    instr_impl_0xD5,
    instr_impl_0xD6,
    instr_impl_0xD7,
    instr_impl_0xD8,
    instr_impl_0xD9,
    instr_impl_0xDA,
    instr_impl_0xDB,
    instr_impl_0xDC,
    instr_impl_0xDD,
    instr_impl_0xDE,
    instr_impl_0xDF,
    instr_impl_0xE0,
    instr_impl_0xE1,
    instr_impl_0xE2,
    instr_impl_0xE3,
    instr_impl_0xE4,
    instr_impl_0xE5,
    instr_impl_0xE6,
    instr_impl_0xE7,
    instr_impl_0xE8,
    instr_impl_0xE9,
    instr_impl_0xEA,
    instr_impl_0xEB,
    instr_impl_0xEC,
    instr_impl_0xED,
    instr_impl_0xEE,
    instr_impl_0xEF,
    instr_impl_0xF0,
    instr_impl_0xF1,
    instr_impl_0xF2,
    instr_impl_0xF3,
    instr_impl_0xF4,
    instr_impl_0xF5,
    instr_impl_0xF6,
    instr_impl_0xF7,
    instr_impl_0xF8,
    instr_impl_0xF9,
    instr_impl_0xFA,
    instr_impl_0xFB,
    instr_impl_0xFC,
    instr_impl_0xFD,
    instr_impl_0xFE,
    instr_impl_0xFF,
};
const instr_impl_fn INSTR_IMPL_PREFIX[256] = {
    instr_impl_prefix_0x00,
    instr_impl_prefix_0x01,
    instr_impl_prefix_0x02,
    instr_impl_prefix_0x03,
    instr_impl_prefix_0x04,
    instr_impl_prefix_0x05,
    instr_impl_prefix_0x06,
    instr_impl_prefix_0x07,
    instr_impl_prefix_0x08,
    instr_impl_prefix_0x09,
    instr_impl_prefix_0x0A,
    instr_impl_prefix_0x0B,
    instr_impl_prefix_0x0C,
    instr_impl_prefix_0x0D,
    instr_impl_prefix_0x0E,
    instr_impl_prefix_0x0F,
    instr_impl_prefix_0x10,
    instr_impl_prefix_0x11,
    instr_impl_prefix_0x12,
    instr_impl_prefix_0x13,
    instr_impl_prefix_0x14,
    instr_impl_prefix_0x15,
    instr_impl_prefix_0x16,
    instr_impl_prefix_0x17,
    instr_impl_prefix_0x18,
    instr_impl_prefix_0x19,
    instr_impl_prefix_0x1A,
    instr_impl_prefix_0x1B,
    instr_impl_prefix_0x1C,
    instr_impl_prefix_0x1D,
    instr_impl_prefix_0x1E,
    instr_impl_prefix_0x1F,
    instr_impl_prefix_0x20,
    instr_impl_prefix_0x21,
    instr_impl_prefix_0x22,
    instr_impl_prefix_0x23,
    instr_impl_prefix_0x24,
    instr_impl_prefix_0x25,
    instr_impl_prefix_0x26,
    instr_impl_prefix_0x27,
    instr_impl_prefix_0x28,
    instr_impl_prefix_0x29,
    instr_impl_prefix_0x2A,
    instr_impl_prefix_0x2B,
    instr_impl_prefix_0x2C,
    instr_impl_prefix_0x2D,
    instr_impl_prefix_0x2E,
    instr_impl_prefix_0x2F,
    instr_impl_prefix_0x30,
    instr_impl_prefix_0x31,
    instr_impl_prefix_0x32,
    instr_impl_prefix_0x33,
    instr_impl_prefix_0x34,
    instr_impl_prefix_0x35,
    instr_impl_prefix_0x36,
    instr_impl_prefix_0x37,
    instr_impl_prefix_0x38,
    instr_impl_prefix_0x39,
    instr_impl_prefix_0x3A,
    instr_impl_prefix_0x3B,
    instr_impl_prefix_0x3C,
    instr_impl_prefix_0x3D,
    instr_impl_prefix_0x3E,
    instr_impl_prefix_0x3F,
    instr_impl_prefix_0x40,
    instr_impl_prefix_0x41,
    instr_impl_prefix_0x42,
    instr_impl_prefix_0x43,
    instr_impl_prefix_0x44,
    instr_impl_prefix_0x45,
    instr_impl_prefix_0x46,
    instr_impl_prefix_0x47,
    instr_impl_prefix_0x48,
    instr_impl_prefix_0x49,
    instr_impl_prefix_0x4A,
    instr_impl_prefix_0x4B,
    instr_impl_prefix_0x4C,
    instr_impl_prefix_0x4D,
    instr_impl_prefix_0x4E,
    instr_impl_prefix_0x4F,
    instr_impl_prefix_0x50,
    instr_impl_prefix_0x51,
    instr_impl_prefix_0x52,
    instr_impl_prefix_0x53,
    instr_impl_prefix_0x54,
    instr_impl_prefix_0x55,
    instr_impl_prefix_0x56,
    instr_impl_prefix_0x57,
    instr_impl_prefix_0x58,
    instr_impl_prefix_0x59,
    instr_impl_prefix_0x5A,
    instr_impl_prefix_0x5B,
    instr_impl_prefix_0x5C,
    instr_impl_prefix_0x5D,
    instr_impl_prefix_0x5E,
    instr_impl_prefix_0x5F,
    instr_impl_prefix_0x60,
    instr_impl_prefix_0x61,
    instr_impl_prefix_0x62,
    instr_impl_prefix_0x63,
    instr_impl_prefix_0x64,
    instr_impl_prefix_0x65,
    instr_impl_prefix_0x66,
    instr_impl_prefix_0x67,
    instr_impl_prefix_0x68,
    instr_impl_prefix_0x69,
    instr_impl_prefix_0x6A,
    instr_impl_prefix_0x6B,
    instr_impl_prefix_0x6C,
    instr_impl_prefix_0x6D,
    instr_impl_prefix_0x6E,
    instr_impl_prefix_0x6F,
    instr_impl_prefix_0x70,
    instr_impl_prefix_0x71,
    instr_impl_prefix_0x72,
    instr_impl_prefix_0x73,
    instr_impl_prefix_0x74,
    instr_impl_prefix_0x75,
    instr_impl_prefix_0x76,
    instr_impl_prefix_0x77,
    instr_impl_prefix_0x78,
    instr_impl_prefix_0x79,
    instr_impl_prefix_0x7A,
    instr_impl_prefix_0x7B,
    instr_impl_prefix_0x7C,
    instr_impl_prefix_0x7D,
    instr_impl_prefix_0x7E,
    instr_impl_prefix_0x7F,
    instr_impl_prefix_0x80,
    instr_impl_prefix_0x81,
    instr_impl_prefix_0x82,
    instr_impl_prefix_0x83,
    instr_impl_prefix_0x84,
    instr_impl_prefix_0x85,
    instr_impl_prefix_0x86,
    instr_impl_prefix_0x87,
    instr_impl_prefix_0x88,
    instr_impl_prefix_0x89,
    instr_impl_prefix_0x8A,
    instr_impl_prefix_0x8B,
    instr_impl_prefix_0x8C,
    instr_impl_prefix_0x8D,
    instr_impl_prefix_0x8E,
    instr_impl_prefix_0x8F,
    instr_impl_prefix_0x90,
    instr_impl_prefix_0x91,
    instr_impl_prefix_0x92,
    instr_impl_prefix_0x93,
    instr_impl_prefix_0x94,
    instr_impl_prefix_0x95,
    instr_impl_prefix_0x96,
    instr_impl_prefix_0x97,
    instr_impl_prefix_0x98,
    instr_impl_prefix_0x99,
    instr_impl_prefix_0x9A,
    instr_impl_prefix_0x9B,
    instr_impl_prefix_0x9C,
    instr_impl_prefix_0x9D,
    instr_impl_prefix_0x9E,
    instr_impl_prefix_0x9F,
    instr_impl_prefix_0xA0,
    instr_impl_prefix_0xA1,
    instr_impl_prefix_0xA2,
    instr_impl_prefix_0xA3,
    instr_impl_prefix_0xA4,
    instr_impl_prefix_0xA5,
    instr_impl_prefix_0xA6,
    instr_impl_prefix_0xA7,
    instr_impl_prefix_0xA8,
    instr_impl_prefix_0xA9,
    instr_impl_prefix_0xAA,
    instr_impl_prefix_0xAB,
    instr_impl_prefix_0xAC,
    instr_impl_prefix_0xAD,
    instr_impl_prefix_0xAE,
    instr_impl_prefix_0xAF,
    instr_impl_prefix_0xB0,
    instr_impl_prefix_0xB1,
    instr_impl_prefix_0xB2,
    instr_impl_prefix_0xB3,
    instr_impl_prefix_0xB4,
    instr_impl_prefix_0xB5,
    instr_impl_prefix_0xB6,
    instr_impl_prefix_0xB7,
    instr_impl_prefix_0xB8,
    instr_impl_prefix_0xB9,
    instr_impl_prefix_0xBA,
    instr_impl_prefix_0xBB,
    instr_impl_prefix_0xBC,
    instr_impl_prefix_0xBD,
    instr_impl_prefix_0xBE,
    instr_impl_prefix_0xBF,
    instr_impl_prefix_0xC0,
    instr_impl_prefix_0xC1,
    instr_impl_prefix_0xC2,
    instr_impl_prefix_0xC3,
    instr_impl_prefix_0xC4,
    instr_impl_prefix_0xC5,
    instr_impl_prefix_0xC6,
    instr_impl_prefix_0xC7,
    instr_impl_prefix_0xC8,
    instr_impl_prefix_0xC9,
    instr_impl_prefix_0xCA,
    instr_impl_prefix_0xCB,
    instr_impl_prefix_0xCC,
    instr_impl_prefix_0xCD,
    instr_impl_prefix_0xCE,
    instr_impl_prefix_0xCF,
    instr_impl_prefix_0xD0,
    instr_impl_prefix_0xD1,
    instr_impl_prefix_0xD2,
    instr_impl_prefix_0xD3,
    instr_impl_prefix_0xD4,
    instr_impl_prefix_0xD5,
    instr_impl_prefix_0xD6,
    instr_impl_prefix_0xD7,
    instr_impl_prefix_0xD8,
    instr_impl_prefix_0xD9,
    instr_impl_prefix_0xDA,
    instr_impl_prefix_0xDB,
    instr_impl_prefix_0xDC,
    instr_impl_prefix_0xDD,
    instr_impl_prefix_0xDE,
    instr_impl_prefix_0xDF,
    instr_impl_prefix_0xE0,
    instr_impl_prefix_0xE1,
    instr_impl_prefix_0xE2,
    instr_impl_prefix_0xE3,
    instr_impl_prefix_0xE4,
    instr_impl_prefix_0xE5,
    instr_impl_prefix_0xE6,
    instr_impl_prefix_0xE7,
    instr_impl_prefix_0xE8,
    instr_impl_prefix_0xE9,
    instr_impl_prefix_0xEA,
    instr_impl_prefix_0xEB,
    instr_impl_prefix_0xEC,
    instr_impl_prefix_0xED,
    instr_impl_prefix_0xEE,
    instr_impl_prefix_0xEF,
    instr_impl_prefix_0xF0,
    instr_impl_prefix_0xF1,
    instr_impl_prefix_0xF2,
    instr_impl_prefix_0xF3,
    instr_impl_prefix_0xF4,
    instr_impl_prefix_0xF5,
    instr_impl_prefix_0xF6,
    instr_impl_prefix_0xF7,
    instr_impl_prefix_0xF8,
    instr_impl_prefix_0xF9,
    instr_impl_prefix_0xFA,
    instr_impl_prefix_0xFB,
    instr_impl_prefix_0xFC,
    instr_impl_prefix_0xFD,
    instr_impl_prefix_0xFE,
    instr_impl_prefix_0xFF,
};
