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

struct interrupt_controller {
    uint8_t     master;
    uint8_t     enabled;
    uint8_t     triggered;
    struct cpu  *cpu;
};

void        interrupt_controller_init(struct interrupt_controller *ic, struct cpu *cpu);
void        interrupt_controller_cleanup(struct interrupt_controller *ic);
void        interrupt_controller_trigger(struct interrupt_controller *ic, const enum interrupt i);
void        interrupt_controller_handle(struct interrupt_controller *ic);

uint8_t     interrupt_controller_io_if(const struct interrupt_controller *ic);
uint8_t     interrupt_controller_io_ie(const struct interrupt_controller *ic);

void        interrupt_controller_io_set_if(struct interrupt_controller *ic, const uint8_t v);
void        interrupt_controller_io_set_ie(struct interrupt_controller *ic, const uint8_t v);

#endif
