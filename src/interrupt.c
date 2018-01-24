#include <stdio.h>
#include "interrupt.h"
#include "cpu.h"

void interrupt_controller_init(struct interrupt_controller *ic, struct cpu *cpu) {
    ic->master = 0;
    ic->enabled = 0;
    ic->triggered = 0;
    ic->cpu = cpu;
}

void interrupt_controller_cleanup(struct interrupt_controller *ic) {
    (void)ic;
}

void interrupt_controller_trigger(struct interrupt_controller *ic, const enum interrupt i) {
    ic->triggered |= (i & 0x1F);
}

void interrupt_controller_handle(struct interrupt_controller *ic) {
    /* Only run if master is set or CPU is halting, and there are anything in IF and IE. */
    if((ic->master || ic->cpu->halt) && ic->enabled && ic->triggered) {

        /* Iterate according to priority. */
        for(int n = 4; n >= 0; n--) {
            int flag = (1 << n);
            if((ic->enabled & flag) && (ic->triggered & flag)) {
                if(ic->master) {
                    ic->master = 0;
                    ic->triggered &= ~flag;
                    cpu_push(ic->cpu, ic->cpu->pc);
                    ic->cpu->pc = (uint16_t)(0x0040 + (n * 0x08));
                    if(ic->cpu->halt) {
                        ic->cpu->halt = 0;
                    }
                    break;
                } else if(ic->cpu->halt) {
                    ic->cpu->halt = 0;
                    break;
                }
            }
        }
    }
}

uint8_t interrupt_controller_io_if(const struct interrupt_controller *ic) {
    return ic->triggered;
}

uint8_t interrupt_controller_io_ie(const struct interrupt_controller *ic) {
    return ic->enabled;
}

void interrupt_controller_io_set_if(struct interrupt_controller *ic, const uint8_t v) {
    ic->triggered = v & 0x1F;
}

void interrupt_controller_io_set_ie(struct interrupt_controller *ic, const uint8_t v) {
    ic->enabled = v & 0x1F;
}
