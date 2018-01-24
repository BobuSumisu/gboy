#include <string.h>
#include <stdio.h>
#include "timer.h"
#include "interrupt.h"

/*** Public ***/

void timer_init(struct timer *timer, struct interrupt_controller *ic) {
    memset(timer, 0, sizeof(struct timer));
    timer->ic = ic;
}

void timer_cleanup(struct timer *timer) {
    (void)timer;
}

void timer_update(struct timer *timer, uint16_t cycles) {
    uint16_t old_counter = timer->counter;
    timer->counter += cycles;

    if((timer->reg_tac & 0x04) == 0) {
        /* Don't update TIMA if timer is disabled. */
        timer->counter += cycles;
        return;
    }

    int n = 0;
    switch(timer->reg_tac & 0x03) {
        case 1: n = 4; break;               /* f/2^4  = 262,144 Hz */
        case 2: n = 6; break;               /* f/2^6  =  65,536 Hz */
        case 3: n = 8; break;               /* f/2^8  =  16,384 Hz */
        case 0: n = 10; break;              /* f/2^10 =   4,096 Hz */
    }

    int flag = (1 << (n - 1));

    /* Check if bit is turned on in this step. */
    if((timer->counter & flag) != 0 && (old_counter & flag) == 0) {
        timer->reg_tima++;
        if(timer->reg_tima == 0) {
            timer->reg_tima = timer->reg_tma;
            interrupt_controller_trigger(timer->ic, INT_TIMER);
        }
    }
}

uint8_t timer_io_div(const struct timer *timer) {
    /* DIV is the upper 8-bit of the counter. */
    return timer->counter >> 8;
}

uint8_t timer_io_tima(const struct timer *timer) {
    return timer->reg_tima;
}

uint8_t timer_io_tma(const struct timer *timer) {
    return timer->reg_tma;
}

uint8_t timer_io_tac(const struct timer *timer) {
    return timer->reg_tac;
}

void timer_io_set_div(struct timer *timer, const uint8_t v) {
    (void)v;
    /* Clear the upper 8 bits of the counter. */
    /* timer->counter &= 0x00FF; */
    /* According to some sources (TCAGBD.pdf) the whole internal counter
     * is cleared on write to DIV. */
    timer->counter = 0;
}

void timer_io_set_tima(struct timer *timer, const uint8_t v) {
    timer->reg_tima = v;
}

void timer_io_set_tma(struct timer *timer, const uint8_t v) {
    timer->reg_tma = v;
}

void timer_io_set_tac(struct timer *timer, const uint8_t v) {
    timer->reg_tac = v & 0x07;
}
