#include <stdio.h>
#include <assert.h>
#include "alu.h"

uint16_t alu_add_u16_i8(struct cpu *cpu, const uint16_t a, const int8_t b) {
    int32_t r = (int32_t)a + b;
    cpu_set_flag(cpu, FLAG_Z, 0);
    cpu_set_flag(cpu, FLAG_N, 0);
    cpu_set_flag(cpu, FLAG_H, ((a ^ b ^ r) & 0x10) != 0);
    cpu_set_flag(cpu, FLAG_C, ((a ^ b ^ r) & 0x100) != 0);
    return (uint16_t)r;
}

uint16_t alu_add16(struct cpu *cpu, const uint16_t a, const uint16_t b) {
    uint32_t r = a + b;
    cpu_set_flag(cpu, FLAG_N, 0);
    cpu_set_flag(cpu, FLAG_H, ((a ^ b ^ r) & 0x1000) != 0);
    cpu_set_flag(cpu, FLAG_C, (r & 0x10000) != 0);
    return (uint16_t)r;
}

uint8_t alu_add(struct cpu *cpu, const uint8_t a, const uint8_t b) {
    uint16_t r = a + b;
    cpu_set_flag(cpu, FLAG_Z, (uint8_t)r == 0);
    cpu_set_flag(cpu, FLAG_N, 0);
    cpu_set_flag(cpu, FLAG_H, ((a ^ b ^ r) & 0x10) != 0);
    cpu_set_flag(cpu, FLAG_C, (r & 0x100) != 0);
    return (uint8_t)r;
}

uint8_t alu_adc(struct cpu *cpu, const uint8_t a, const uint8_t b) {
    uint8_t c = cpu_flag(cpu, FLAG_C);
    uint16_t r = a + b + c;
    cpu_set_flag(cpu, FLAG_Z, (uint8_t)r == 0);
    cpu_set_flag(cpu, FLAG_N, 0);
    cpu_set_flag(cpu, FLAG_H, ((a ^ b ^ r) & 0x10) != 0);
    cpu_set_flag(cpu, FLAG_C, (r & 0x100) != 0);
    return (uint8_t)r;
}

uint8_t alu_sub(struct cpu *cpu, const uint8_t a, const uint8_t b) {
    uint8_t r = a - b;
    cpu_set_flag(cpu, FLAG_Z, r == 0);
    cpu_set_flag(cpu, FLAG_N, 1);
    cpu_set_flag(cpu, FLAG_H, ((a ^ b ^ r) & 0x10) != 0);
    cpu_set_flag(cpu, FLAG_C, (a - b) < 0);
    return r;
}

uint8_t alu_sbc(struct cpu *cpu, const uint8_t a, const uint8_t b) {
    uint32_t r = a - b - cpu_flag(cpu, FLAG_C);
    cpu_set_flag(cpu, FLAG_Z, (uint8_t)r == 0);
    cpu_set_flag(cpu, FLAG_N, 1);
    cpu_set_flag(cpu, FLAG_H, ((a ^ b ^ r) & 0x10) != 0);
    cpu_set_flag(cpu, FLAG_C, (r & 0x100) != 0);
    return (uint8_t)r;
}

uint8_t alu_and(struct cpu *cpu, const uint8_t a, const uint8_t b) {
    uint8_t r = a & b;
    cpu_set_flag(cpu, FLAG_Z, r == 0);
    cpu_set_flag(cpu, FLAG_N, 0);
    cpu_set_flag(cpu, FLAG_H, 1);
    cpu_set_flag(cpu, FLAG_C, 0);
    return r;
}

uint8_t alu_xor(struct cpu *cpu, const uint8_t a, const uint8_t b) {
    uint8_t r = a ^ b;
    cpu_set_flag(cpu, FLAG_Z, r == 0);
    cpu_set_flag(cpu, FLAG_N, 0);
    cpu_set_flag(cpu, FLAG_H, 0);
    cpu_set_flag(cpu, FLAG_C, 0);
    return r;
}

uint8_t alu_or(struct cpu *cpu, const uint8_t a, const uint8_t b) {
    uint8_t r = a | b;
    cpu_set_flag(cpu, FLAG_Z, r == 0);
    cpu_set_flag(cpu, FLAG_N, 0);
    cpu_set_flag(cpu, FLAG_H, 0);
    cpu_set_flag(cpu, FLAG_C, 0);
    return r;
}

void alu_cp(struct cpu *cpu, const uint8_t a, const uint8_t b) {
    alu_sub(cpu, a, b);
}

uint8_t alu_inc(struct cpu *cpu, const uint8_t a) {
    uint8_t r = a + 1;
    cpu_set_flag(cpu, FLAG_Z, r == 0);
    cpu_set_flag(cpu, FLAG_N, 0);
    cpu_set_flag(cpu, FLAG_H, (((a & 0x0F) + 1) & 0x10) == 0x10);
    return r;
}

uint8_t alu_dec(struct cpu *cpu, const uint8_t a) {
    uint8_t r = a - 1;
    cpu_set_flag(cpu, FLAG_Z, r == 0);
    cpu_set_flag(cpu, FLAG_N, 1);
    cpu_set_flag(cpu, FLAG_H, (a & 0x0F) == 0);
    return r;
}

uint8_t alu_daa(struct cpu *cpu, const uint8_t a) {
    uint8_t b = 0;
    uint16_t r;

    if(cpu->f & FLAG_H) {
        b |= 0x06;
    }

    if(cpu->f & FLAG_C) {
        b |= 0x60;
    }

    if(cpu->f & FLAG_N) {
        r = a - b;
    } else {
        if((a & 0x0F) > 0x09) {
            b |= 0x06;
        }
        if(a > 0x99) {
            b |= 0x60;
        }
        r = a + b;
    }

    cpu_set_flag(cpu, FLAG_Z, (uint8_t)r == 0);
    cpu_set_flag(cpu, FLAG_H, 0);
    cpu_set_flag(cpu, FLAG_C, (b & 0x60) != 0);
    return (uint8_t)r;
}

uint8_t alu_cpl(struct cpu *cpu, const uint8_t a) {
    uint8_t r = ~a;
    cpu_set_flag(cpu, FLAG_N, 1);
    cpu_set_flag(cpu, FLAG_H, 1);
    return r;
}

void alu_scf(struct cpu *cpu) {
    cpu_set_flag(cpu, FLAG_N, 0);
    cpu_set_flag(cpu, FLAG_H, 0);
    cpu_set_flag(cpu, FLAG_C, 1);
}

void alu_ccf(struct cpu *cpu) {
    cpu_set_flag(cpu, FLAG_N, 0);
    cpu_set_flag(cpu, FLAG_H, 0);
    cpu_set_flag(cpu, FLAG_C, !cpu_flag(cpu, FLAG_C));
}

void alu_bit(struct cpu *cpu, const uint8_t a, const uint8_t n) {
    assert(n < 8);
    cpu_set_flag(cpu, FLAG_Z, (a & (1 << n)) == 0);
    cpu_set_flag(cpu, FLAG_N, 0);
    cpu_set_flag(cpu, FLAG_H, 1);
}

uint8_t alu_res(struct cpu *cpu, const uint8_t a, const uint8_t n) {
    assert(n < 8);
    (void)cpu;
    return a & ~(1 << n);
}

uint8_t alu_set(struct cpu *cpu, const uint8_t a, const uint8_t n) {
    assert(n < 8);
    (void)cpu;
    return (uint8_t)(a | (1 << n));
}

uint8_t alu_rlc(struct cpu *cpu, const uint8_t a) {
    uint8_t r = (uint8_t)((a << 1) | (a >> 7));
    cpu_set_flag(cpu, FLAG_Z, r == 0);
    cpu_set_flag(cpu, FLAG_N, 0);
    cpu_set_flag(cpu, FLAG_H, 0);
    cpu_set_flag(cpu, FLAG_C, (a & 0x80) == 0x80);
    return r;
}

uint8_t alu_rrc(struct cpu *cpu, const uint8_t a) {
    uint8_t r = (uint8_t)((a << 7) | (a >> 1));
    cpu_set_flag(cpu, FLAG_Z, r == 0);
    cpu_set_flag(cpu, FLAG_N, 0);
    cpu_set_flag(cpu, FLAG_H, 0);
    cpu_set_flag(cpu, FLAG_C, (a & 0x01) == 0x01);
    return r;
}

uint8_t alu_rl(struct cpu *cpu, const uint8_t a) {
    uint8_t r = (uint8_t)(a << 1) | cpu_flag(cpu, FLAG_C);
    cpu_set_flag(cpu, FLAG_Z, r == 0);
    cpu_set_flag(cpu, FLAG_N, 0);
    cpu_set_flag(cpu, FLAG_H, 0);
    cpu_set_flag(cpu, FLAG_C, (a & 0x80) == 0x80);
    return r;
}

uint8_t alu_rr(struct cpu *cpu, const uint8_t a) {
    uint8_t r = (uint8_t)((cpu_flag(cpu, FLAG_C) << 7) | (a >> 1));
    cpu_set_flag(cpu, FLAG_Z, r == 0);
    cpu_set_flag(cpu, FLAG_N, 0);
    cpu_set_flag(cpu, FLAG_H, 0);
    cpu_set_flag(cpu, FLAG_C, (a & 0x01) == 0x01);
    return r;
}

uint8_t alu_sla(struct cpu *cpu, const uint8_t a) {
    uint8_t r = (uint8_t)(a << 1);
    cpu_set_flag(cpu, FLAG_Z, r == 0);
    cpu_set_flag(cpu, FLAG_N, 0);
    cpu_set_flag(cpu, FLAG_H, 0);
    cpu_set_flag(cpu, FLAG_C, (a & 0x80) == 0x80);
    return r;
}

uint8_t alu_sra(struct cpu *cpu, const uint8_t a) {
    uint8_t r = (uint8_t)((a & 0x80) | (a >> 1));
    cpu_set_flag(cpu, FLAG_Z, r == 0);
    cpu_set_flag(cpu, FLAG_N, 0);
    cpu_set_flag(cpu, FLAG_H, 0);
    cpu_set_flag(cpu, FLAG_C, (a & 0x01) == 0x01);
    return r;
}

uint8_t alu_swap(struct cpu *cpu, const uint8_t a) {
    uint8_t r = (uint8_t)((a << 4) | (a >> 4));
    cpu_set_flag(cpu, FLAG_Z, r == 0);
    cpu_set_flag(cpu, FLAG_N, 0);
    cpu_set_flag(cpu, FLAG_H, 0);
    cpu_set_flag(cpu, FLAG_C, 0);
    return r;
}

uint8_t alu_srl(struct cpu *cpu, const uint8_t a) {
    uint8_t r = (uint8_t)(a >> 1);
    cpu_set_flag(cpu, FLAG_Z, r == 0);
    cpu_set_flag(cpu, FLAG_N, 0);
    cpu_set_flag(cpu, FLAG_H, 0);
    cpu_set_flag(cpu, FLAG_C, (a & 0x01) == 0x01);
    return r;
}
