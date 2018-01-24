#include <stdio.h>
#include "interrupt.h"
#include "cpu.h"

void interrupts_init(struct interrupts *interrupts, struct cpu *cpu) {
    interrupts->master = 0;
    interrupts->enabled = 0;
    interrupts->triggered = 0;
    interrupts->cpu = cpu;
}

void interrupts_cleanup(struct interrupts *interrupts) {
    (void)interrupts;
}

void interrupts_trigger(struct interrupts *interrupts, const enum interrupt inter) {
    interrupts->triggered |= (inter & 0x1F);
}

void interrupts_handle(struct interrupts *interrupts) {
    /* Only run if master is set or CPU is halting, and there are anything in IF and IE. */
    if((interrupts->master || interrupts->cpu->halt) && interrupts->enabled && interrupts->triggered) {

        /* Iterate according to priority. */
        for(int n = 4; n >= 0; n--) {
            int flag = (1 << n);
            if((interrupts->enabled & flag) && (interrupts->triggered & flag)) {

                /*  Have to check both if master interrupt flag (IME) is set,
                 *  and wether the CPU is in HALT mode.
                 *
                 *  If IME is set then we should jump to the interrupt handler in
                 *  both normal and HALT mode (and reset halt if it was set).
                 *
                 *  If IME is not set then we shouldn't do anything execpt if
                 *  the CPU is in HALT mode, in which case we continue
                 *  execution from the halt instruction.
                 *
                 *  TODO: It seems like the interrupt flag should not be reset
                 *  when IME = 0 and in HALT mode??
                 */
                if(interrupts->master) {
                    interrupts->master = 0;
                    interrupts->triggered &= ~flag;
                    cpu_push(interrupts->cpu, interrupts->cpu->pc);
                    interrupts->cpu->pc = (uint16_t)(0x0040 + (n * 0x08));
                    if(interrupts->cpu->halt) {
                        interrupts->cpu->halt = 0;
                    }
                    break;
                } else if(interrupts->cpu->halt) {
                    // interrupts->triggered &= ~flag;
                    interrupts->cpu->halt = 0;
                    break;
                }
            }
        }
    }
}
