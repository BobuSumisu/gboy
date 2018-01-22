/*
 *  sound.h
 *  =======
 *
 *  Some abstractions to make it easier to work with audio.
 *
 */
#ifndef GBOY_SOUND_H
#define GBOY_SOUND_H

#include <inttypes.h>

struct sound {
    uint8_t     on;

    float       len;
    uint8_t     duty;
    uint16_t    freq;
    float       freq_hz;
    uint8_t     one_shot;

    uint8_t     env_step_len;
    uint8_t     env_dir;
    uint8_t     env_step_level;
    float       env_out;

    uint8_t     sweep_shift;
    uint8_t     sweep_dir;
    uint8_t     sweep_time;

    float       clock;

    uint8_t     out_so1;
    uint8_t     out_so2;
};

void    sound_set_sweep(struct sound *sound, const uint8_t b);
void    sound_set_len(struct sound *sound, const uint8_t b);
void    sound_set_env(struct sound *sound, const uint8_t b);
void    sound_set_lo(struct sound *sound, const uint8_t b);
void    sound_set_hi(struct sound *sound, const uint8_t b);
void    sound_update(struct sound *sound, const float ms);
float   sound_generate(struct sound *sound, const int sample);

#endif
