#ifndef __GAMEBOY_INSTR_INFO_H__
#define __GAMEBOY_INSTR_INFO_H__

#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>

enum op {
    OP_0,
    OP_00H,
    OP_08H,
    OP_1,
    OP_10H,
    OP_18H,
    OP_2,
    OP_20H,
    OP_28H,
    OP_3,
    OP_30H,
    OP_38H,
    OP_4,
    OP_5,
    OP_6,
    OP_7,
    OP_A,
    OP_A16,
    OP_A16_ADDR,
    OP_A8_ADDR,
    OP_AF,
    OP_B,
    OP_BC,
    OP_BC_ADDR,
    OP_C,
    OP_CB,
    OP_C_ADDR,
    OP_D,
    OP_D16,
    OP_D8,
    OP_DE,
    OP_DE_ADDR,
    OP_E,
    OP_H,
    OP_HL,
    OP_HLD_ADDR,
    OP_HLI_ADDR,
    OP_HL_ADDR,
    OP_L,
    OP_NC,
    OP_NONE,
    OP_NZ,
    OP_R8,
    OP_SP,
    OP_Z,
};

enum op_type {
    OP_TYPE_CONST,
    OP_TYPE_FLAG,
    OP_TYPE_IMM16,
    OP_TYPE_IMM16_ADDR,
    OP_TYPE_IMM8,
    OP_TYPE_IMM8_ADDR,
    OP_TYPE_NONE,
    OP_TYPE_REG16,
    OP_TYPE_REG16_ADDR,
    OP_TYPE_REG8,
    OP_TYPE_REG8_ADDR,
};

struct instr_info {
    uint8_t        opcode;
    const char     *desc;
    const char     *mnem;
    enum op        dst;
    enum op_type   dst_type;
    enum op        src;
    enum op_type   src_type;
    uint8_t        size;
    uint8_t        cycles;
    uint8_t        cond_cycles;
    char           flags[4];
};

extern const struct instr_info INSTR_INFOS[256];
extern const struct instr_info INSTR_INFOS_PREFIX[256];

#endif
