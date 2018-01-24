#include <stdio.h>
#include "input.h"
#include "bitutil.h"

void input_init(struct input *input) {
    input->sel = 0x30;
    input->col1 = 0xCF;
    input->col2 = 0xCF;
}

void input_cleanup(struct input *input) {
    (void)input;
}

uint8_t input_io_p1(const struct input *input) {
    switch(input->sel) {
        case 0x10: return input->col2;
        case 0x20: return input->col1;
        case 0x00:
        default: return 0xCF;
    }
}

void input_io_set_p1(struct input *input, const uint8_t v) {
    input->sel = v & 0x30;
}

void input_keydown(struct input *input, enum key key) {
    switch(key) {
        case KEY_RIGHT:     input->col1 &= 0xCE; break;
        case KEY_LEFT:      input->col1 &= 0xCD; break;
        case KEY_UP:        input->col1 &= 0xCB; break;
        case KEY_DOWN:      input->col1 &= 0xC7; break;

        case KEY_A:         input->col2 &= 0xCE; break;
        case KEY_B:         input->col2 &= 0xCD; break;
        case KEY_SELECT:    input->col2 &= 0xCB; break;
        case KEY_START:     input->col2 &= 0xC7; break;
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
