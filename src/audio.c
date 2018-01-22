#include <string.h>
#include <math.h>
#include "audio.h"

#define SAMPLES_PER_SEC     48000
#define CHANNELS            1
#define AUDIO_BUFSIZ        4096

#define CYCLES_PER_SEC      4194304
#define MS_PER_CYCLE        0.00023841857f

float square_wave(int sample, float freq, float volume, int duty) {
    float period = SAMPLES_PER_SEC / freq;
    float pos = (float)fmod(sample, period) / period;

    switch(duty) {
        case 0: /* 12.5% */
            return (pos < 0.125) ? -volume : volume;
        case 1: /* 25% */
            return (pos < 0.25) ? -volume : volume;
        case 2: /* 50% */
            return (pos < 0.50) ? -volume : volume;
        case 3: /* 75% */
            return (pos < 0.75) ? -volume : volume;
    }

    return 0;
}

float freq_to_hz(uint16_t freq) {
    return (4194304.0f / (32.0f * (2048.0f - freq)));
}

void sdl_audio_callback(void *user, uint8_t *stream, int len) {
    struct audio *audio = (struct audio *)user;
    float *fstream = (float *)((void *)stream);

    memset(stream, 0, (size_t)len);

    for(int i = 0; i < len / 4; i++) {
        int sample = audio->sample++;

        float so1 = 0;

        if(audio->sound1.out_so1) {
            so1 += square_wave(sample, freq_to_hz(audio->sound1.freq), audio->sound1.env,
                audio->sound1.duty);
        }
        if(audio->sound2.out_so1) {
            float tmp = square_wave(sample, freq_to_hz(audio->sound2.freq), 1.0,
                audio->sound2.duty);
            so1 = (so1 * 0.5f) + (tmp * 0.5f);
        }

        fstream[i] = so1;
    }
}

void audio_init(struct audio *audio) {
    memset(audio, 0, sizeof(struct audio));
    audio->reg_nr52 = 0xF1;

    SDL_AudioSpec want, have;
    memset(&want, 0, sizeof(SDL_AudioSpec));
    want.freq = SAMPLES_PER_SEC;
    want.format = AUDIO_F32;
    want.channels = CHANNELS;
    want.samples = AUDIO_BUFSIZ;
    want.callback = sdl_audio_callback;
    want.userdata = audio;

    audio->dev = SDL_OpenAudioDevice(NULL, 0, &want, &have, SDL_AUDIO_ALLOW_FORMAT_CHANGE);
    if(audio->dev == 0) {
        fprintf(stderr, "SDL_OpenAudioDevice failed: %s\n", SDL_GetError());
        return;
    }

    SDL_PauseAudioDevice(audio->dev, 0);
}

void audio_cleanup(struct audio *audio) {
    if(audio->dev != 0) {
        SDL_CloseAudioDevice(audio->dev);
    }
}

void sound_update(struct sound *sound, float ms) {
    sound->clock += ms;

    if(sound->one_shot && sound->clock >= sound->len) {
        return;
    }

    /* Envelope */
    if(sound->env_sweeps != 0) {
        float step_time = sound->env_sweeps * (1000.0f / 64.0f);
        float step_ratio = 1.0f / 8.0f;
        int curr_step = (int)(sound->clock / step_time);
        if(curr_step <= 8) {
            if(sound->env_dir) {
                sound->env = curr_step * step_ratio;
            } else {
                sound->env = 1.0f - (curr_step * step_ratio);
            }
        }
    }
}

void audio_update(struct audio *audio, int cycles) {
    float ms = cycles * MS_PER_CYCLE;
    audio->clock += cycles;
    sound_update(&audio->sound1, ms);
    sound_update(&audio->sound2, ms);
}

uint8_t audio_rb(struct audio *audio, const uint16_t addr) {
    switch(addr) {
        case 0xFF10: return audio->reg_nr10;
        case 0xFF11: return audio->reg_nr11;
        case 0xFF12: return audio->reg_nr12;
        case 0xFF13: return audio->reg_nr13;
        case 0xFF14: return audio->reg_nr14;

        case 0xFF16: return audio->reg_nr21;
        case 0xFF17: return audio->reg_nr22;
        case 0xFF18: return audio->reg_nr23;
        case 0xFF19: return audio->reg_nr24;

        case 0xFF24: return audio->reg_nr50;
        case 0xFF25: return audio->reg_nr51;
        case 0xFF26: return audio->reg_nr52;
    }
    return 0;
}

void sound_len_duty(struct sound *sound, const uint8_t b) {
    sound->duty = b >> 6;
    int len = b & 0x3F;
    sound->len = (64 - len) * (1000.0f / 256.0f);
}

void sound_freq_lo(struct sound *sound, const uint8_t b) {
    sound->freq = (sound->freq & 0xFF00) | b;
}

void sound_freq_hi(struct sound *sound, const uint8_t b) {
    uint8_t hi = b & 0x07;
    sound->freq = (uint16_t)(hi << 8 | (sound->freq & 0x00FF));
    sound->one_shot = b & 0x40;
    if(b & 0x80) {
        sound->clock = 0;
    }
}

void sound_env(struct sound *sound, const uint8_t b) {
    int init = b >> 4;
    sound->env_init = init / 15.0f;
    sound->env = sound->env_init;
    sound->env_dir = b & 0x08;
    sound->env_sweeps = b & 0x07;
}

void audio_wb(struct audio *audio, const uint16_t addr, const uint8_t b) {
    // printf("audio_wb: 0x%04X <-- 0x%02X\n", addr, b);
    switch(addr) {
        case 0xFF10: audio->reg_nr10 = b; break;
        case 0xFF11: audio->reg_nr11 = b; sound_len_duty(&audio->sound1, b); break;
        case 0xFF12: audio->reg_nr12 = b; sound_env(&audio->sound1, b); break;
        case 0xFF13: audio->reg_nr13 = b; sound_freq_lo(&audio->sound1, b); break;
        case 0xFF14: audio->reg_nr14 = b; sound_freq_hi(&audio->sound1, b); break;

        case 0xFF16: audio->reg_nr21 = b; sound_len_duty(&audio->sound2, b); break;
        case 0xFF17: audio->reg_nr22 = b; sound_env(&audio->sound2, b); break;
        case 0xFF18: audio->reg_nr23 = b; sound_freq_lo(&audio->sound2, b); break;
        case 0xFF19: audio->reg_nr24 = b; sound_freq_hi(&audio->sound2, b); break;

        case 0xFF24: audio->reg_nr50 = b; break;
        case 0xFF25:
            audio->reg_nr51 = b;
            audio->sound1.out_so1 = (b & 0x01) == 0x01;
            audio->sound2.out_so1 = (b & 0x02) == 0x02;
            audio->sound1.out_so2 = (b & 0x10) == 0x01;
            audio->sound2.out_so2 = (b & 0x20) == 0x02;
            break;
        case 0xFF26: audio->reg_nr52 = b; break;
    }
}
