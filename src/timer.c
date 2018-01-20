#include <string.h>
#include "timer.h"
#include "cpu.h"

#define TAC_TIMER_ON 0x04

/*** Private ***/

static void timer_check(struct timer *timer) {
    if(timer->reg_tac & TAC_TIMER_ON) {
        int resolution = 1;
        switch(timer->reg_tac & 0x03) {
            case 0:
                resolution = 64;    /* 4.096 KHz = 1/64 base clock */
                break;
            case 1:
                resolution = 1;     /* 262.144 KHz = base clock */
                break;
            case 2:
                resolution = 4;     /* 65.536 KHz  = 1/4 base clock */
                break;
            case 3:
                resolution = 16;    /* 16.384 KHz = 1/16 base clock */
                break;
        }

        if(timer->clock >= resolution) {
            timer->clock -= resolution;
            timer->reg_tima++;
            /* If overflow set to modulo. */
            if(timer->reg_tima == 0) {
                timer->reg_tima = timer->reg_tma;
                timer->cpu->reg_if |= INT_TIMER;
            }
        }
    }
}

/*** Public ***/

void timer_init(struct timer *timer, struct cpu *cpu) {
    memset(timer, 0, sizeof(struct timer));
    timer->cpu = cpu;
}

void timer_cleanup(struct timer *timer) {
    (void)timer;
}

void timer_update(struct timer *timer, int cycles) {
    timer->t_clock += cycles;
    while(timer->t_clock >= 4) {
        timer->t_clock -= 4;
        timer->m_clock++;

        if(timer->m_clock == 4) {
            timer->m_clock = 0;
            timer->clock++;
            timer->div_timer++;

            /* div updates 1/16 of base clock. */
            if(timer->div_timer == 16) {
                timer->reg_div++;
                timer->div_timer = 0;
            }
        }
    }

    timer_check(timer);
}
