#ifndef GBOY_APU_H
#define GBOY_APU_H

#include <inttypes.h>
#include <SDL2/SDL.h>
#include "sound.h"

#define SAMPLES_PER_SEC     48000
#define CHANNELS            1
#define AUDIO_BUFSIZ        4096

struct apu {
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

void        apu_init(struct apu *apu);
void        apu_cleanup(struct apu *apu);
void        apu_update(struct apu *apu, const int cycles);
uint8_t     apu_rb(struct apu *apu, const uint16_t addr);
void        apu_wb(struct apu *apu, const uint16_t addr, const uint8_t b);

uint8_t     apu_io_nr10(const struct apu *apu);
uint8_t     apu_io_nr11(const struct apu *apu);
uint8_t     apu_io_nr12(const struct apu *apu);
uint8_t     apu_io_nr13(const struct apu *apu);
uint8_t     apu_io_nr14(const struct apu *apu);

uint8_t     apu_io_nr21(const struct apu *apu);
uint8_t     apu_io_nr22(const struct apu *apu);
uint8_t     apu_io_nr23(const struct apu *apu);
uint8_t     apu_io_nr24(const struct apu *apu);

uint8_t     apu_io_nr30(const struct apu *apu);
uint8_t     apu_io_nr31(const struct apu *apu);
uint8_t     apu_io_nr32(const struct apu *apu);
uint8_t     apu_io_nr33(const struct apu *apu);
uint8_t     apu_io_nr34(const struct apu *apu);

uint8_t     apu_io_nr41(const struct apu *apu);
uint8_t     apu_io_nr42(const struct apu *apu);
uint8_t     apu_io_nr43(const struct apu *apu);
uint8_t     apu_io_nr44(const struct apu *apu);

uint8_t     apu_io_nr50(const struct apu *apu);
uint8_t     apu_io_nr51(const struct apu *apu);
uint8_t     apu_io_nr52(const struct apu *apu);

uint8_t     apu_io_set_nr10(struct apu *apu, const uint8_t v);
uint8_t     apu_io_set_nr11(struct apu *apu, const uint8_t v);
uint8_t     apu_io_set_nr12(struct apu *apu, const uint8_t v);
uint8_t     apu_io_set_nr13(struct apu *apu, const uint8_t v);
uint8_t     apu_io_set_nr14(struct apu *apu, const uint8_t v);

uint8_t     apu_io_set_nr21(struct apu *apu, const uint8_t v);
uint8_t     apu_io_set_nr22(struct apu *apu, const uint8_t v);
uint8_t     apu_io_set_nr23(struct apu *apu, const uint8_t v);
uint8_t     apu_io_set_nr24(struct apu *apu, const uint8_t v);

uint8_t     apu_io_set_nr30(struct apu *apu, const uint8_t v);
uint8_t     apu_io_set_nr31(struct apu *apu, const uint8_t v);
uint8_t     apu_io_set_nr32(struct apu *apu, const uint8_t v);
uint8_t     apu_io_set_nr33(struct apu *apu, const uint8_t v);
uint8_t     apu_io_set_nr34(struct apu *apu, const uint8_t v);

uint8_t     apu_io_set_nr41(struct apu *apu, const uint8_t v);
uint8_t     apu_io_set_nr42(struct apu *apu, const uint8_t v);
uint8_t     apu_io_set_nr43(struct apu *apu, const uint8_t v);
uint8_t     apu_io_set_nr44(struct apu *apu, const uint8_t v);

uint8_t     apu_io_set_nr50(struct apu *apu, const uint8_t v);
uint8_t     apu_io_set_nr51(struct apu *apu, const uint8_t v);
uint8_t     apu_io_set_nr52(struct apu *apu, const uint8_t v);

#endif
