#ifndef GBOY_INTERRUPT_H
#define GBOY_INTERRUPT_H

#include <inttypes.h>

enum interrupt {
    INT_VBLANK  = 0x01,
    INT_LCDC    = 0x02,
    INT_TIMER   = 0x04,
    INT_SERIAL  = 0x08,
    INT_INPUT   = 0x10,
};

struct interrupts {
    uint8_t master;
    uint8_t enabled;
    uint8_t triggered;

    struct cpu *cpu;
};

void interrupts_init(struct interrupts *interrupts, struct cpu *cpu);
void interrupts_cleanup(struct interrupts *interrupts);
void interrupts_trigger(struct interrupts *interrupts, const enum interrupt inter);
void interrupts_handle(struct interrupts *interrupts);

#endif
