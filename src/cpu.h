#ifndef GBOY_CPU_H
#define GBOY_CPU_H

#include <inttypes.h>

#define FLAG_Z 0x80
#define FLAG_N 0x40
#define FLAG_H 0x20
#define FLAG_C 0x10

struct cpu {
    union {
        struct {
            uint8_t f;
            uint8_t a;
        };
        uint16_t af;
    };
    union {
        struct {
            uint8_t c;
            uint8_t b;
        };
        uint16_t bc;
    };
    union {
        struct {
            uint8_t e;
            uint8_t d;
        };
        uint16_t de;
    };
    union {
        struct {
            uint8_t l;
            uint8_t h;
        };
        uint16_t hl;
    };
    uint16_t pc;
    uint16_t sp;
    struct mmu *mmu;
};

void        cpu_init(struct cpu *cpu, struct mmu *mmu);
void        cpu_free(struct cpu *cpu);
int         cpu_step(struct cpu *cpu);
void        cpu_run(struct cpu *cpu);
uint8_t     cpu_flag(struct cpu *cpu, uint8_t flag);
void        cpu_set_flag(struct cpu *cpu, uint8_t flag, int cond);

#endif
