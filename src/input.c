#include <stdio.h>
#include "input.h"
#include "bitutil.h"

void input_init(struct input *input) {
    input->sel = 0x30;
    input->col1 = 0x0F;
    input->col2 = 0x0F;
}

void input_cleanup(struct input *input) {
    (void)input;
}

uint8_t input_rb(struct input *input) {
    switch(input->sel) {
        case 0x10: return input->col2;
        case 0x20: return input->col1;
        case 0x00:
        default: return 0xFF;
    }
}

void input_wb(struct input *input, const uint8_t v) {
    input->sel = v & 0x30;
}

void input_keydown(struct input *input, enum key key) {
    switch(key) {
        case KEY_RIGHT:     input->col1 &= 0x0E; break;
        case KEY_LEFT:      input->col1 &= 0x0D; break;
        case KEY_UP:        input->col1 &= 0x0B; break;
        case KEY_DOWN:      input->col1 &= 0x07; break;

        case KEY_A:         input->col2 &= 0x0E; break;
        case KEY_B:         input->col2 &= 0x0D; break;
        case KEY_SELECT:    input->col2 &= 0x0B; break;
        case KEY_START:     input->col2 &= 0x07; break;
    }
}

void input_keyup(struct input *input, enum key key) {
    switch(key) {
        case KEY_RIGHT:     input->col1 |= 0x01; break;
        case KEY_LEFT:      input->col1 |= 0x02; break;
        case KEY_UP:        input->col1 |= 0x04; break;
        case KEY_DOWN:      input->col1 |= 0x08; break;

        case KEY_A:         input->col2 |= 0x01; break;
        case KEY_B:         input->col2 |= 0x02; break;
        case KEY_SELECT:    input->col2 |= 0x04; break;
        case KEY_START:     input->col2 |= 0x08; break;
    }
}
