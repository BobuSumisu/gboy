#include <stdio.h>
#include <string.h>

#include "cpu.h"
#include "mmu.h"
#include "instr.h"

/*** Private ***/

static void cpu_int_handler(struct cpu *cpu) {
    if(cpu->ime && cpu->reg_ie && cpu->reg_if) {
        for(int i = 0; i < 4; i++) {
            int flag = (1 << i);
            if((cpu->reg_ie & flag) && (cpu->reg_if & flag)) {
                cpu->ime = 0;
                cpu->reg_if &= ~flag;
                cpu_push(cpu, cpu->pc);
                cpu->pc = (uint16_t)(0x0040 + (i * 0x08));
            }
        }
    }
}

/*** Public ***/

void cpu_init(struct cpu *cpu, struct mmu *mmu) {
    memset(cpu, 0, sizeof(struct cpu));
    cpu->mmu = mmu;
}

void cpu_cleanup(struct cpu *cpu) {
    (void)cpu;
}

int cpu_step(struct cpu *cpu) {
    int cycles = instr_fetch_decode_execute(cpu);
    cpu_int_handler(cpu);
    return cycles;
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

uint8_t cpu_fb(struct cpu *cpu) {
    return mmu_rb(cpu->mmu, cpu->pc++);
}

uint16_t cpu_fw(struct cpu *cpu) {
    uint16_t w = mmu_rw(cpu->mmu, cpu->pc);
    cpu->pc += 2;
    return w;
}

void cpu_push(struct cpu *cpu, uint16_t w) {
    cpu->sp -= 2;
    mmu_ww(cpu->mmu, cpu->sp, w);
}

uint16_t cpu_pop(struct cpu *cpu) {
    uint16_t w = mmu_rw(cpu->mmu, cpu->sp);
    cpu->sp += 2;
    return w;
}

void cpu_debug(struct cpu *cpu) {
    printf("+----+----+----+----+----+----+----+----+------+------+-----------------");
    printf("+---------------------+---------------------+\n");
    printf("| A  | F  | B  | C  | D  | E  | H  | L  |  PC  |  SP  |      FLAGS      ");
    printf("|         IE          |          IF         |\n");
    printf("+----+----+----+----+----+----+----+----+------+------+-----------------");
    printf("+---------------------+---------------------+\n");
    printf("| %02X | %02X | %02X | %02X | %02X | %02X | %02X | %02X | %04X | %04X "
            "| Z:%d N:%d H:%d C:%d "
            "| V:%d L:%d T:%d S:%d I:%d "
            "| V:%d L:%d T:%d S:%d I:%d |\n",
        cpu->a, cpu->f, cpu->b, cpu->c,
        cpu->d, cpu->e, cpu->h, cpu->l,
        cpu->pc, cpu->sp,
        cpu_flag(cpu, FLAG_Z),
        cpu_flag(cpu, FLAG_N),
        cpu_flag(cpu, FLAG_H),
        cpu_flag(cpu, FLAG_C),
        (cpu->reg_ie & INT_VBLANK) != 0,
        (cpu->reg_ie & INT_LCDC) != 0,
        (cpu->reg_ie & INT_TIMER) != 0,
        (cpu->reg_ie & INT_SERIAL) != 0,
        (cpu->reg_ie & INT_INPUT) != 0,
        (cpu->reg_if & INT_VBLANK) != 0,
        (cpu->reg_if & INT_LCDC) != 0,
        (cpu->reg_if & INT_TIMER) != 0,
        (cpu->reg_if & INT_SERIAL) != 0,
        (cpu->reg_if & INT_INPUT) != 0);
    printf("+----+----+----+----+----+----+----+----+------+------+-----------------");
    printf("+---------------------+---------------------+\n");
}


