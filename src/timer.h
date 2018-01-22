#ifndef GBOY_TIMER_H
#define GBOY_TIMER_H

#include <inttypes.h>

struct timer {
    uint8_t reg_div;        /* 0xFF04 */
    uint8_t reg_tima;       /* 0xFF05 */
    uint8_t reg_tma;        /* 0xFF06 */
    uint8_t reg_tac;        /* 0xFF07 */

    int t_clock;            /* 4,194.304 KHz */
    int m_clock;            /* 1,048.576 KHz */
    int clock;              /*   262.144 KHz */

    int div_timer;          /* Updates at 1/16 base clock */

    struct interrupts *interrupts;
};

void timer_init(struct timer *timer, struct interrupts *interrupts);
void timer_cleanup(struct timer *timer);
void timer_update(struct timer *timer, int cycles);

#endif
