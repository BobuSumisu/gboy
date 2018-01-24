#ifndef GBOY_TIMER_H
#define GBOY_TIMER_H

#include <inttypes.h>

struct timer {
    uint8_t reg_div;        /* 0xFF04 */
    uint8_t reg_tima;       /* 0xFF05 */
    uint8_t reg_tma;        /* 0xFF06 */
    uint8_t reg_tac;        /* 0xFF07 */

    /*
     *  Use a single counter which counts the base clock frequency (f = 4,194,304 Hz).
     *
     *      bit 15  = f/2^16    =          64 Hz ---+
     *      bit 14  = f/2^15    =         128 Hz    |
     *      bit 13  = f/2^14    =         256 Hz    |
     *      bit 12  = f/2^13    =         512 Hz    | DIV register (16,384 Hz)
     *      bit 11  = f/2^12    =       1,024 Hz    |
     *      bit 10  = f/2^11    =       2,048 Hz    |
     *      bit 9   = f/2^10    =       4,096 Hz    |       <-- TAC 00
     *      bit 8   = f/2^9     =       8,192 Hz ---+
     *      bit 7   = f/2^8     =      16,384 Hz            <-- TAC 11
     *      bit 6   = f/2^7     =      32,768 Hz
     *      bit 5   = f/2^6     =      65,536 Hz            <-- TAC 10
     *      bit 4   = f/2^5     =     131,072 Hz
     *      bit 3   = f/2^4     =     262,144 Hz            <-- TAC 01
     *      bit 2   = f/2^3     =     524,288 Hz
     *      bit 1   = f/2^2     =   1,048,576 Hz
     *      bit 0   = f/2^1     =   2,097,152 Hz
     *
     *  This internal counter is used as the source for updating TIMA.
     *
     */
    uint16_t counter;

    struct interrupt_controller *ic;
};

void timer_init(struct timer *timer, struct interrupt_controller *ic);
void timer_cleanup(struct timer *timer);
void timer_update(struct timer *timer, uint16_t cycles);

uint8_t timer_io_div(const struct timer *timer);
uint8_t timer_io_tima(const struct timer *timer);
uint8_t timer_io_tma(const struct timer *timer);
uint8_t timer_io_tac(const struct timer *timer);

void timer_io_set_div(struct timer *timer, const uint8_t v);
void timer_io_set_tima(struct timer *timer, const uint8_t v);
void timer_io_set_tma(struct timer *timer, const uint8_t v);
void timer_io_set_tac(struct timer *timer, const uint8_t v);

#endif
