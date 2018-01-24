#include <string.h>
#include <math.h>
#include "audio.h"

#define CYCLES_PER_SEC      4194304
#define MS_PER_CYCLE        0.00023841857f

static float mix(float a, float b) {
    return (a + b) / 2;
}

void sdl_audio_callback(void *user, uint8_t *stream, int len) {
    struct audio *audio = (struct audio *)user;
    float *fstream = (float *)((void *)stream);

    memset(stream, 0, (size_t)len);

    for(int i = 0; i < len / 4; i++) {
        int sample = audio->sample++;

        float out = 1.0;

        if(audio->sound1.on) {
            out = mix(out, sound_generate(&audio->sound1, sample));
        }

        if(audio->sound2.on) {
            out = mix(out, sound_generate(&audio->sound2, sample));
        }

        fstream[i] = out * 0.01f;
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

void audio_update(struct audio *audio, int cycles) {
    float ms = cycles * MS_PER_CYCLE;
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

        case 0xFF1A: return audio->reg_nr30;
        case 0xFF1B: return audio->reg_nr31;
        case 0xFF1C: return audio->reg_nr32;
        case 0xFF1D: return audio->reg_nr33;
        case 0xFF1E: return audio->reg_nr34;

        case 0xFF20: return audio->reg_nr41;
        case 0xFF21: return audio->reg_nr42;
        case 0xFF22: return audio->reg_nr43;
        case 0xFF23: return audio->reg_nr44;

        case 0xFF24: return audio->reg_nr50;
        case 0xFF25: return audio->reg_nr51;
        case 0xFF26: return audio->reg_nr52;
    }
    return 0;
}

void audio_wb(struct audio *audio, const uint16_t addr, const uint8_t b) {

    /* Ignore writes to registers when sound is off. */
    if((audio->reg_nr52 & 0x80) == 0 && addr != 0xFF26) {
        return;
    }

    switch(addr) {
        case 0xFF10:
            audio->reg_nr10 = b & 0x7F;
            sound_set_sweep(&audio->sound1, b);
            break;
        case 0xFF11:
            audio->reg_nr11 = b;
            sound_set_len(&audio->sound1, b);
            break;
        case 0xFF12:
            audio->reg_nr12 = b;
            sound_set_env(&audio->sound1, b);
            break;
        case 0xFF13:
            audio->reg_nr13 = b;
            sound_set_lo(&audio->sound1, b);
            break;
        case 0xFF14:
            audio->reg_nr14 = b & 0xC7;
            sound_set_hi(&audio->sound1, b);
            break;

        case 0xFF16:
            audio->reg_nr21 = b;
            sound_set_len(&audio->sound2, b);
            break;
        case 0xFF17:
            audio->reg_nr22 = b;
            sound_set_env(&audio->sound2, b);
            break;
        case 0xFF18:
            audio->reg_nr23 = b;
            sound_set_lo(&audio->sound2, b);
            break;
        case 0xFF19:
            audio->reg_nr24 = b & 0xC7;
            sound_set_hi(&audio->sound2, b);
            break;

        case 0xFF1A: audio->reg_nr30 = b & 0x80; break;
        case 0xFF1B: audio->reg_nr31 = b; break;
        case 0xFF1C: audio->reg_nr32 = b & 0x40; break;
        case 0xFF1D: audio->reg_nr33 = b; break;
        case 0xFF1E: audio->reg_nr34 = b & 0xC7; break;

        case 0xFF20: audio->reg_nr41 = b & 0x3F; break;
        case 0xFF21: audio->reg_nr42 = b; break;
        case 0xFF22: audio->reg_nr43 = b; break;
        case 0xFF23: audio->reg_nr44 = b & 0xC0; break;

        case 0xFF24:
            audio->reg_nr50 = b;
            break;
        case 0xFF25:
            audio->reg_nr51 = b;
            audio->sound1.out_so1 = (b & 0x01);
            audio->sound2.out_so1 = (b & 0x02);
            audio->sound1.out_so2 = (b & 0x10);
            audio->sound2.out_so2 = (b & 0x20);
            break;
        case 0xFF26:
            audio->reg_nr52 = b & 0x8F;
            break;
    }
}
