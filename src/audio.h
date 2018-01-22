/*
 *  audio.h
 *  =======
 *
 *  Audio generation using SDL.
 *
 *  CPU time (t-clock) is in 4194304 Hz.
 *
 *  Sweep time specified in relation to 128 Hz:
 *
 *      000:
 *      001:  7.8ms (1/128 Hz)
 *      010: 15.6ms (2/128 Hz)
 *      011: 23.4ms (3/128 Hz)
 *      100: 31.3ms (4/128 Hz)
 *      101: 39.1ms (5/128 Hz)
 *      110: 46.9ms (6/128 Hz)
 *      111: 54.7ms (7/128 Hz)
 *
 *  Change in frequency: X(t) = X(t - 1) +/- X(t - 1) / 2^n     t=sweep time, n=sweep shifts
 *
 *  Sound length = (64 - t1) * (1 / 256) seconds.
 *
 *  Envelope step = n * (1 / 64) seconds.
 *
 */
#ifndef GBOY_AUDIO_H
#define GBOY_AUDIO_H

#include <inttypes.h>
#include <SDL2/SDL.h>
#include "sound.h"

#define SAMPLES_PER_SEC     48000
#define CHANNELS            1
#define AUDIO_BUFSIZ        4096

struct audio {
    uint8_t reg_nr10;       /* 0xFF10 - Sound 1 Sweep */
    uint8_t reg_nr11;       /* 0xFF11 - Sound 1 Wave */
    uint8_t reg_nr12;       /* 0xFF12 - Sound 1 Envelope */
    uint8_t reg_nr13;       /* 0xFF13 - Sound 1 Frequency Low */
    uint8_t reg_nr14;       /* 0xFF14 - Sound 1 Frequency High */

    uint8_t reg_nr21;       /* 0xFF16 - Sound 2 Wave */
    uint8_t reg_nr22;       /* 0xFF17 - Sound 2 Envelope */
    uint8_t reg_nr23;       /* 0xFF18 - Sound 2 Frequency Low */
    uint8_t reg_nr24;       /* 0xFF19 - Sound 2 Frequency High */

    uint8_t reg_nr30;       /* 0xFF1A - Sound 3 On */
    uint8_t reg_nr31;       /* 0xFF1B - Sound 3 Length */
    uint8_t reg_nr32;       /* 0xFF1C - Sound 3 Level */
    uint8_t reg_nr33;       /* 0xFF1D - Sound 3 Frequency Low */
    uint8_t reg_nr34;       /* 0xFF1E - Sound 3 Frequency High */

    uint8_t reg_nr41;       /* 0xFF20 - Sound 4 Length */
    uint8_t reg_nr42;       /* 0xFF21 - Sound 4 Envelope */
    uint8_t reg_nr43;       /* 0xFF22 - Sound 4 Polynomial */
    uint8_t reg_nr44;       /* 0xFF23 - Sound 4 Counter */

    uint8_t reg_nr50;       /* 0xFF24 - Channel control */
    uint8_t reg_nr51;       /* 0xFF25 - Output selection */
    uint8_t reg_nr52;       /* 0xFF26 - Sound on/off */

    uint8_t wave_ram[0x10]; /* 0xFF30 - 0xFF40 */

    SDL_AudioDeviceID dev;

    uint16_t sample;

    struct sound sound1;
    struct sound sound2;
};

void    audio_init(struct audio *audio);
void    audio_cleanup(struct audio *audio);
void    audio_update(struct audio *audio, const int cycles);
uint8_t audio_rb(struct audio *audio, const uint16_t addr);
void    audio_wb(struct audio *audio, const uint16_t addr, const uint8_t b);

#endif
