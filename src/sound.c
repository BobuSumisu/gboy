#include <string.h>
#include "sound.h"

#define SAMPLES_PER_SEC     48000
#define CHANNELS            2
#define AUDIO_BUFSIZ        4096

float square_wave(int sample, float freq, float volume) {
    float period = SAMPLES_PER_SEC / freq;
    float half_period = period / 2;
    return ((sample / (int)half_period) % 2) ? volume : -volume;
}

void sound_init(struct sound *sound) {
    memset(sound, 0, sizeof(struct sound));
    sound->reg_nr52 = 0xF1;

    SDL_AudioSpec want, have;
    memset(&want, 0, sizeof(SDL_AudioSpec));
    want.freq = SAMPLES_PER_SEC;
    want.format = AUDIO_F32;
    want.channels = CHANNELS;
    want.samples = AUDIO_BUFSIZ;
    want.callback = NULL;

    sound->dev = SDL_OpenAudioDevice(NULL, 0, &want, &have, SDL_AUDIO_ALLOW_FORMAT_CHANGE);
    if(sound->dev == 0) {
        fprintf(stderr, "SDL_OpenAudioDevice failed: %s\n", SDL_GetError());
        return;
    }

    SDL_PauseAudioDevice(sound->dev, 0);
    int bytes_per_sample = sizeof(float) * CHANNELS;
    int num_samples = 48000 / 4;

    void *buf = malloc((size_t)(num_samples * bytes_per_sample));
    float *ptr = buf;

    for(int sample = 0; sample < num_samples; sample++) {
        float value = square_wave(sample, 440.0f, 0.05f);
        *ptr++ = value;
        *ptr++ = value;
    }
    SDL_QueueAudio(sound->dev, buf, (uint32_t)(num_samples * bytes_per_sample));
    free(buf);
}

void sound_cleanup(struct sound *sound) {
    if(sound->dev != 0) {
        SDL_CloseAudioDevice(sound->dev);
    }
}

void sound_update(struct sound *sound, int cycles) {
    (void)sound;
    (void)cycles;

    if(sound->reg_nr52 & 0x80) {
        int so1 = sound->reg_nr50 & 0x07;
        if(so1 > 0) {
            if(sound->reg_nr51 & 0x01) {
                printf("sound1 -> SO1\n");
            }
        }
        int so2 = sound->reg_nr50 & 0x07;
        if(so2 > 0) {
            if(sound->reg_nr51 & 0x10) {
                printf("sound1 -> SO2\n");
            }
        }
    }

}

static void wave_sound_set_sweep(struct wave_sound *snd, const uint8_t b) {
    snd->sweep_time = (b >> 4) & 0x07;
    snd->sweep_dir = (b >> 3) & 0x01;
    snd->sweep_shift = (b >> 0) & 0x03;
}

static void wave_sound_set_length(struct wave_sound *snd, const uint8_t b) {
    snd->wave_duty = (b >> 6) & 0x03;
    snd->wave_len = b & 0x3F;
}

static void wave_sound_set_env(struct wave_sound *snd, const uint8_t b) {
    snd->env_vol = (b >> 4) & 0x0F;
    snd->env_dir = (b >> 3) & 0x01;
    snd->env_sweep = b & 0x03;
}

static void wave_sound_set_freq_lo(struct wave_sound *snd, const uint8_t b) {
    snd->freq = (snd->freq & 0xFF00) | b;
}

static void wave_sound_set_freq_hi(struct wave_sound *snd, const uint8_t b) {
    uint8_t hi = b & 0x07;
    snd->freq = (uint16_t)((hi << 8) | (snd->freq & 0x00FF));
    snd->consec = (b >> 7) & 0x01;
    snd->initial = (b >> 6) & 0x01;
}

uint8_t sound_rb(struct sound *sound, const uint16_t addr) {
    switch(addr) {
        case 0xFF10: return sound->reg_nr10;
        case 0xFF11: return sound->reg_nr11;
        case 0xFF12: return sound->reg_nr12;
        case 0xFF13: return sound->reg_nr13;
        case 0xFF14: return sound->reg_nr14;
        case 0xFF24: return sound->reg_nr50;
        case 0xFF25: return sound->reg_nr51;
        case 0xFF26: return sound->reg_nr52;
    }
    return 0;
}

void sound_wb(struct sound *sound, const uint16_t addr, const uint8_t b) {
    switch(addr) {
        case 0xFF10: sound->reg_nr10 = b; wave_sound_set_sweep(&sound->sound1, b); break;
        case 0xFF11: sound->reg_nr11 = b; wave_sound_set_length(&sound->sound1, b); break;
        case 0xFF12: sound->reg_nr12 = b; wave_sound_set_env(&sound->sound1, b); break;
        case 0xFF13: sound->reg_nr13 = b; wave_sound_set_freq_lo(&sound->sound1, b); break;
        case 0xFF14: sound->reg_nr14 = b; wave_sound_set_freq_hi(&sound->sound1, b); break;
        case 0xFF24: sound->reg_nr50 = b; break;
        case 0xFF25: sound->reg_nr51 = b; break;
        case 0xFF26: sound->reg_nr52 = b; break;
    }
}
