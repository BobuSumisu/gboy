#include <math.h>
#include <stdio.h>
#include "sound.h"
#include "apu.h"

static float freq_to_hz(const uint16_t freq) {
    return (4194304.0f / (32.0f * (2048.0f - freq)));
}

static float square_wave(int sample, float freq, int duty) {
    float period = SAMPLES_PER_SEC / freq;
    float pos = (float)fmod(sample, period) / period;

    /*
     * 12.5%    = -1-- ---- | -1-- ----
     * 25.0%    = 11-- ---- | 11-- ----
     * 50.0%    = 1111 ---- | 1111 ----
     * 75.0%    = --11 1111 | --11 1111
     */
    switch(duty) {
        case 0: /* 12.5% = */
            return (pos >= 0.125 && pos < 0.25) ? 1.0f : -1.0f;
        case 1: /* 25.0% */
            return (pos < 0.25) ? 1.0f : -1.0f;
        case 2: /* 50.0% */
            return (pos < 0.50) ? 1.0f : -1.0f;
        case 3: /* 75.0% */
            return (pos >= 0.25) ? 1.0f : -1.0f;
    }

    return 0;
}

void sound_set_sweep(struct sound *sound, const uint8_t b) {
    sound->sweep_shift = b & 0x07;
    sound->sweep_dir = b & 0x08;
    sound->sweep_time = (b >> 4) & 0x07;
}

void sound_set_len(struct sound *sound, const uint8_t b) {
    uint8_t len = b & 0x3F;
    sound->len = (64 - len) * (1000.0f / 256.0f);
    sound->duty = (b >> 6) & 0x03;
}

void sound_set_env(struct sound *sound, const uint8_t b) {
    sound->env_step_len = b & 0x07;
    sound->env_dir = b & 0x08;
    sound->env_step_level = (b >> 4) & 0x0F;
}

void sound_set_lo(struct sound *sound, const uint8_t b) {
    sound->freq = (sound->freq & 0xFF00) | b;
    sound->freq_hz = freq_to_hz(sound->freq);
}

void sound_set_hi(struct sound *sound, const uint8_t b) {
    uint8_t hi = b & 0x07;
    sound->freq = (uint16_t)((hi << 8) | (sound->freq & 0x00FF));
    sound->freq_hz = freq_to_hz(sound->freq);
    sound->one_shot = b & 0x40;
    if(b & 0x80) {
        sound->clock = 0;
    }
}

void sound_update(struct sound *sound, const float ms) {
    sound->clock += ms;

    if(sound->one_shot && sound->clock > sound->len) {
        sound->on = 0;
        return;
    }

    sound->on = 1;

    /* Update sweep */
    if(sound->sweep_time > 0) {
        float time = sound->sweep_time / (1000.0f / 128.0f);
        (void)time;
        /* TODO: implement */
    }

    /* Update envelope. */
    if(sound->env_step_level == 0) {
        sound->env_out = 0.0f;
    } else {
        if(sound->env_step_len == 0) {
            sound->env_out = 1.0f;
        } else {
            float step_ms = sound->env_step_len * (1000.0f / 64.0f);
            float step_val = 1.0f / sound->env_step_level;
            int curr_step = (int)(sound->clock / step_ms);
            if(curr_step < sound->env_step_level) {
                if(sound->env_dir) {
                    sound->env_out = step_val * curr_step;
                } else {
                    sound->env_out = 1.0f - (step_val * curr_step);
                }
            } else {
                sound->env_out = sound->env_dir ? 1.0f : 0.0f;
            }
        }
    }
}

float sound_generate(struct sound *sound, const int sample) {
    /* Generate square wave sample. */
    float out = square_wave(sample, sound->freq_hz, sound->duty);

    /* Apply envelope. */
    out *= sound->env_out;

    return out;
}
