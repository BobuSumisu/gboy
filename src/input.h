/*
 *  input.h
 *  =======
 *
 *  Emulate the quite confusing P1 joypad register.
 *
 *  P1 consists of 4 input ports and 2 output ports:
 *
 *      bit 7 - not used
 *      bit 6 - not used
 *      bit 5 - P15
 *      bit 4 - P14
 *      bit 3 - P13
 *      bit 2 - P12
 *      bit 1 - P11
 *      bit 0 - P10
 *
 *  The input ports are pulled high.
 *
 *         VDD         P14         P15
 *          |           |           |
 *          |           |           |
 *  P10 ----+-----------+-(R)-------+-(A)-------
 *          |           |           |
 *  P11 ----+-----------+-(L)-------+-(B)-------
 *          |           |           |
 *  P12 ----+-----------+-(U)-------+-(Select)--
 *          |           |           |
 *  P13 ----+-----------+-(D)-------+-(Start)---
 *
 *  To read input, turn on P14 (0x10) or P15 (0x20) to select line,
 *  then (after a brief interval) read back to get the P10-P13.
 *
 *  Turn on both (0x30) to reset?
 *
 *  Interrupt (0x10) on negative edge input on P10-P13.
 *  Interrupt also on input to the RESET terminal with P10-P13 in LOW state.
 *
 */
#ifndef GBOY_INPUT_H
#define GBOY_INPUT_H

#include <inttypes.h>

enum key {
    KEY_RIGHT,
    KEY_LEFT,
    KEY_UP,
    KEY_DOWN,
    KEY_A,
    KEY_B,
    KEY_SELECT,
    KEY_START,
};

struct input {
    uint8_t sel;
    uint8_t col1;
    uint8_t col2;
};

void    input_init(struct input *input);
void    input_cleanup(struct input *input);
uint8_t input_rb(struct input *input);
void    input_wb(struct input *input, const uint8_t v);

void    input_keydown(struct input *input, enum key key);
void    input_keyup(struct input *input, enum key key);

#endif
