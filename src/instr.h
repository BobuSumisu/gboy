/*
 *  instr.h
 *  =======
 *
 *  Implementation of CPU instructions and fetch-decode-execute cycle.
 */
#ifndef GBOY_INSTR_H
#define GBOY_INSTR_H

#include "cpu.h"

int instr_fetch_decode_execute(struct cpu *cpu);

#endif
