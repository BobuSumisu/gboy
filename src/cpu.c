#include <stdio.h>
#include <string.h>

#include "cpu.h"
#include "interrupt.h"
#include "mmu.h"
#include "instr_info.h"
#include "instr_impl.h"

#if PRINT_DEBUG == 1
#define DEBUG_STEP(n) do { \
    printf("%04X: ", cpu->pc - n); \
    for(uint16_t i = 0; i < 10; i++) { \
        if(i < info->size) { \
            printf("%02X ", mmu_rb(cpu->mmu, cpu->pc - n + i)); \
        } else { \
            printf("   "); \
        } \
    } \
} while(0)
#else
#define DEBUG_STEP(...)
#endif

/*** Public ***/

void cpu_init(struct cpu *cpu, struct mmu *mmu, struct interrupts *interrupts) {
    memset(cpu, 0, sizeof(struct cpu));
    cpu->mmu = mmu;
    cpu->interrupts = interrupts;
}

void cpu_cleanup(struct cpu *cpu) {
    (void)cpu;
}

int cpu_step(struct cpu *cpu) {
    uint8_t opcode = cpu_fb(cpu);
    const struct instr_info *info = &INSTR_INFO[opcode];
    instr_impl_fn impl = INSTR_IMPL[opcode];

    if(opcode == 0xCB) {
        opcode = cpu_fb(cpu);
        info = &INSTR_INFO_PREFIX[opcode];
        impl = INSTR_IMPL_PREFIX[opcode];
        DEBUG_STEP(2);
    } else {
        DEBUG_STEP(1);
    }

    int cycles = impl(cpu, info);
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
        (cpu->interrupts->enabled & INT_VBLANK) != 0,
        (cpu->interrupts->enabled & INT_LCDC) != 0,
        (cpu->interrupts->enabled & INT_TIMER) != 0,
        (cpu->interrupts->enabled & INT_SERIAL) != 0,
        (cpu->interrupts->enabled & INT_INPUT) != 0,
        (cpu->interrupts->triggered & INT_VBLANK) != 0,
        (cpu->interrupts->triggered & INT_LCDC) != 0,
        (cpu->interrupts->triggered & INT_TIMER) != 0,
        (cpu->interrupts->triggered & INT_SERIAL) != 0,
        (cpu->interrupts->triggered & INT_INPUT) != 0);
    printf("+----+----+----+----+----+----+----+----+------+------+-----------------");
    printf("+---------------------+---------------------+\n");
    printf("IME: %d\n", cpu->interrupts->master);
}
