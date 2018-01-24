#include <string.h>
#include <math.h>
#include "apu.h"

#define CYCLES_PER_SEC      4194304
#define MS_PER_CYCLE        0.00023841857f

static float mix(float a, float b) {
    return (a + b) / 2;
}

void sdl_apu_callback(void *user, uint8_t *stream, int len) {
    struct apu *apu = (struct apu *)user;
    float *fstream = (float *)((void *)stream);

    memset(stream, 0, (size_t)len);

    for(int i = 0; i < len / 4; i++) {
        int sample = apu->sample++;

        float out = 1.0;

        if(apu->sound1.on) {
            out = mix(out, sound_generate(&apu->sound1, sample));
        }

        if(apu->sound2.on) {
            out = mix(out, sound_generate(&apu->sound2, sample));
        }

        fstream[i] = out * 0.01f;
    }
}

void apu_init(struct apu *apu) {
    memset(apu, 0, sizeof(struct apu));
    apu->reg_nr52 = 0xF1;

    SDL_AudioSpec want, have;
    memset(&want, 0, sizeof(SDL_AudioSpec));
    want.freq = SAMPLES_PER_SEC;
    want.format = AUDIO_F32;
    want.channels = CHANNELS;
    want.samples = AUDIO_BUFSIZ;
    want.callback = sdl_apu_callback;
    want.userdata = apu;

    apu->dev = SDL_OpenAudioDevice(NULL, 0, &want, &have, SDL_AUDIO_ALLOW_FORMAT_CHANGE);
    if(apu->dev == 0) {
        fprintf(stderr, "SDL_OpenAudioDevice failed: %s\n", SDL_GetError());
        return;
    }

    SDL_PauseAudioDevice(apu->dev, 0);
}

void apu_cleanup(struct apu *apu) {
    if(apu->dev != 0) {
        SDL_CloseAudioDevice(apu->dev);
    }
}

void apu_update(struct apu *apu, int cycles) {
    float ms = cycles * MS_PER_CYCLE;
    sound_update(&apu->sound1, ms);
    sound_update(&apu->sound2, ms);
}

uint8_t apu_rb(struct apu *apu, const uint16_t addr) {
    switch(addr) {
        case 0xFF10: return apu->reg_nr10;
        case 0xFF11: return apu->reg_nr11;
        case 0xFF12: return apu->reg_nr12;
        case 0xFF13: return apu->reg_nr13;
        case 0xFF14: return apu->reg_nr14;

        case 0xFF16: return apu->reg_nr21;
        case 0xFF17: return apu->reg_nr22;
        case 0xFF18: return apu->reg_nr23;
        case 0xFF19: return apu->reg_nr24;

        case 0xFF1A: return apu->reg_nr30;
        case 0xFF1B: return apu->reg_nr31;
        case 0xFF1C: return apu->reg_nr32;
        case 0xFF1D: return apu->reg_nr33;
        case 0xFF1E: return apu->reg_nr34;

        case 0xFF20: return apu->reg_nr41;
        case 0xFF21: return apu->reg_nr42;
        case 0xFF22: return apu->reg_nr43;
        case 0xFF23: return apu->reg_nr44;

        case 0xFF24: return apu->reg_nr50;
        case 0xFF25: return apu->reg_nr51;
        case 0xFF26: return apu->reg_nr52;
    }
    return 0;
}

void apu_wb(struct apu *apu, const uint16_t addr, const uint8_t b) {

    /* Ignore writes to registers when sound is off. */
    if((apu->reg_nr52 & 0x80) == 0 && addr != 0xFF26) {
        return;
    }

    switch(addr) {
        case 0xFF10:
            apu->reg_nr10 = b & 0x7F;
            sound_set_sweep(&apu->sound1, b);
            break;
        case 0xFF11:
            apu->reg_nr11 = b;
            sound_set_len(&apu->sound1, b);
            break;
        case 0xFF12:
            apu->reg_nr12 = b;
            sound_set_env(&apu->sound1, b);
            break;
        case 0xFF13:
            apu->reg_nr13 = b;
            sound_set_lo(&apu->sound1, b);
            break;
        case 0xFF14:
            apu->reg_nr14 = b & 0xC7;
            sound_set_hi(&apu->sound1, b);
            break;

        case 0xFF16:
            apu->reg_nr21 = b;
            sound_set_len(&apu->sound2, b);
            break;
        case 0xFF17:
            apu->reg_nr22 = b;
            sound_set_env(&apu->sound2, b);
            break;
        case 0xFF18:
            apu->reg_nr23 = b;
            sound_set_lo(&apu->sound2, b);
            break;
        case 0xFF19:
            apu->reg_nr24 = b & 0xC7;
            sound_set_hi(&apu->sound2, b);
            break;

        case 0xFF1A: apu->reg_nr30 = b & 0x80; break;
        case 0xFF1B: apu->reg_nr31 = b; break;
        case 0xFF1C: apu->reg_nr32 = b & 0x40; break;
        case 0xFF1D: apu->reg_nr33 = b; break;
        case 0xFF1E: apu->reg_nr34 = b & 0xC7; break;

        case 0xFF20: apu->reg_nr41 = b & 0x3F; break;
        case 0xFF21: apu->reg_nr42 = b; break;
        case 0xFF22: apu->reg_nr43 = b; break;
        case 0xFF23: apu->reg_nr44 = b & 0xC0; break;

        case 0xFF24:
            apu->reg_nr50 = b;
            break;
        case 0xFF25:
            apu->reg_nr51 = b;
            apu->sound1.out_so1 = (b & 0x01);
            apu->sound2.out_so1 = (b & 0x02);
            apu->sound1.out_so2 = (b & 0x10);
            apu->sound2.out_so2 = (b & 0x20);
            break;
        case 0xFF26:
            apu->reg_nr52 = b & 0x8F;
            break;
    }
}
