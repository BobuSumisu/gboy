#ifndef GBOY_ALU_H
#define GBOY_ALU_H

#include <inttypes.h>
#include "cpu.h"

uint16_t    alu_add_u16_i8(struct cpu *cpu, const uint16_t a, const int8_t b);
uint16_t    alu_add16(struct cpu *cpu, const uint16_t a, const uint16_t b);

uint8_t     alu_add(struct cpu *cpu, const uint8_t a, const uint8_t b);
uint8_t     alu_adc(struct cpu *cpu, const uint8_t a, const uint8_t b);
uint8_t     alu_sub(struct cpu *cpu, const uint8_t a, const uint8_t b);
uint8_t     alu_sbc(struct cpu *cpu, const uint8_t a, const uint8_t b);
uint8_t     alu_and(struct cpu *cpu, const uint8_t a, const uint8_t b);
uint8_t     alu_xor(struct cpu *cpu, const uint8_t a, const uint8_t b);
uint8_t     alu_or(struct cpu *cpu, const uint8_t a, const uint8_t b);
void        alu_cp(struct cpu *cpu, const uint8_t a, const uint8_t b);

uint8_t     alu_inc(struct cpu *cpu, const uint8_t a);
uint8_t     alu_dec(struct cpu *cpu, const uint8_t a);

uint8_t     alu_daa(struct cpu *cpu, const uint8_t a);
uint8_t     alu_cpl(struct cpu *cpu, const uint8_t a);
void        alu_scf(struct cpu *cpu);
void        alu_ccf(struct cpu *cpu);

void        alu_bit(struct cpu *cpu, const uint8_t a, const uint8_t n);
uint8_t     alu_res(struct cpu *cpu, const uint8_t a, const uint8_t n);
uint8_t     alu_set(struct cpu *cpu, const uint8_t a, const uint8_t n);

uint8_t     alu_rlc(struct cpu *cpu, const uint8_t a);
uint8_t     alu_rrc(struct cpu *cpu, const uint8_t a);
uint8_t     alu_rl(struct cpu *cpu, const uint8_t a);
uint8_t     alu_rr(struct cpu *cpu, const uint8_t a);
uint8_t     alu_sla(struct cpu *cpu, const uint8_t a);
uint8_t     alu_sra(struct cpu *cpu, const uint8_t a);
uint8_t     alu_swap(struct cpu *cpu, const uint8_t a);
uint8_t     alu_srl(struct cpu *cpu, const uint8_t a);

#endif
