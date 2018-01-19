#include "alu.h"

uint16_t alu_add_u16_i8(struct cpu *cpu, const uint16_t a, const int8_t b) {
    uint16_t r = (uint16_t)(a + b);
    cpu_set_flag(cpu, FLAG_Z, 0);
    cpu_set_flag(cpu, FLAG_N, 0);
    cpu_set_flag(cpu, FLAG_H, (((a & 0x0FFF) + (b & 0x0FFF)) & 0x1000) == 0x1000);
    cpu_set_flag(cpu, FLAG_C, (uint32_t)(a + b) > 0xFFFF);
    return r;
}

uint8_t alu_add(struct cpu *cpu, const uint8_t a, const uint8_t b) {
    uint8_t r = a + b;
    cpu_set_flag(cpu, FLAG_Z, r == 0);
    cpu_set_flag(cpu, FLAG_N, 0);
    cpu_set_flag(cpu, FLAG_H, (((a & 0x0F) + (b & 0x0F)) & 0x10) == 0x10);
    cpu_set_flag(cpu, FLAG_C, (uint16_t)(a + b) > 0xFF);
    return r;
}

uint8_t alu_adc(struct cpu *cpu, const uint8_t a, const uint8_t b) {
    uint8_t c = cpu_flag(cpu, FLAG_C);
    uint8_t r = a + b + c;
    cpu_set_flag(cpu, FLAG_Z, r == 0);
    cpu_set_flag(cpu, FLAG_N, 0);
    cpu_set_flag(cpu, FLAG_H, (((a & 0x0F) + (b & 0x0F) + c) & 0x10) == 0x10);
    cpu_set_flag(cpu, FLAG_C, (uint16_t)(a + b + c) > 0xFF);
    return r;
}

uint8_t alu_sub(struct cpu *cpu, const uint8_t a, const uint8_t b) {
    uint8_t r = a - b;
    cpu_set_flag(cpu, FLAG_Z, r == 0);
    cpu_set_flag(cpu, FLAG_N, 1);
    cpu_set_flag(cpu, FLAG_H, (a & 0x0F) < (b & 0x0F));
    cpu_set_flag(cpu, FLAG_C, a < b);
    return r;
}

uint8_t alu_sbc(struct cpu *cpu, const uint8_t a, const uint8_t b) {
    uint8_t c = cpu_flag(cpu, FLAG_C);
    uint8_t r = a - b - c;
    cpu_set_flag(cpu, FLAG_Z, r == 0);
    cpu_set_flag(cpu, FLAG_N, 1);
    cpu_set_flag(cpu, FLAG_H, ((a - c) & 0x0F) < (b & 0x0F));
    cpu_set_flag(cpu, FLAG_C, (a - c) < b);
    return r;
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
