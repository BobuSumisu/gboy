#ifndef GBOY_SOUND_H
#define GBOY_SOUND_H

#include <inttypes.h>
#include <SDL2/SDL.h>

struct wave_sound {
    uint8_t     sweep_time;
    uint8_t     sweep_dir;
    uint8_t     sweep_shift;
    uint8_t     wave_duty;
    uint8_t     wave_len;
    uint8_t     env_vol;
    uint8_t     env_dir;
    uint8_t     env_sweep;
    uint16_t    freq;
    uint8_t     consec;
    uint8_t     initial;
};

struct sound {
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

    struct wave_sound sound1;
};

void    sound_init(struct sound *sound);
void    sound_cleanup(struct sound *sound);
void    sound_update(struct sound *sound, const int cycles);
uint8_t sound_rb(struct sound *sound, const uint16_t addr);
void    sound_wb(struct sound *sound, const uint16_t addr, const uint8_t b);

#endif
