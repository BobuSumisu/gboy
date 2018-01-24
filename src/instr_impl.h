#ifndef GBOY_INSTR_IMPL_H
#define GBOY_INSTR_IMPL_H

#include "cpu.h"
#include "instr_info.h"

typedef uint16_t (*instr_impl_fn)(struct cpu *cpu, const struct instr_info *info);

extern const instr_impl_fn INSTR_IMPL[256];
extern const instr_impl_fn INSTR_IMPL_PREFIX[256];

#endif
