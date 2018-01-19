#ifndef GBOY_ALU_H
#define GBOY_ALU_H

#include <inttypes.h>
#include "cpu.h"

uint16_t    alu_add_u16_i8(struct cpu *cpu, const uint16_t a, const int8_t b);

uint8_t     alu_add(struct cpu *cpu, const uint8_t a, const uint8_t b);
uint8_t     alu_adc(struct cpu *cpu, const uint8_t a, const uint8_t b);
uint8_t     alu_sub(struct cpu *cpu, const uint8_t a, const uint8_t b);
uint8_t     alu_sbc(struct cpu *cpu, const uint8_t a, const uint8_t b);
uint8_t     alu_and(struct cpu *cpu, const uint8_t a, const uint8_t b);
uint8_t     alu_xor(struct cpu *cpu, const uint8_t a, const uint8_t b);
uint8_t     alu_or(struct cpu *cpu, const uint8_t a, const uint8_t b);
void        alu_cp(struct cpu *cpu, const uint8_t a, const uint8_t b);

#endif
