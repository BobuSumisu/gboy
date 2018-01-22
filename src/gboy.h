/*
 *  gboy.h
 *  ======
 *
 *  The gboy object is the main interface and glue of the application.
 *
 *  It handles initialization and cleanup for all other objects.
 *
 *  gboy is also responsible for initialization of SDL and handling
 *  of SDL events.
 *
 */
#ifndef GBOY_GBOY_H
#define GBOY_GBOY_H

#include "cpu.h"
#include "interrupt.h"
#include "mmu.h"
#include "gpu.h"
#include "screen.h"
#include "timer.h"
#include "input.h"
#include "audio.h"

struct gboy {
    int debug;
    struct cpu cpu;
    struct interrupts interrupts;
    struct mmu mmu;
    struct gpu gpu;
    struct screen screen;
    struct timer timer;
    struct input input;
    struct audio audio;
};

int     gboy_init(struct gboy *gboy);
void    gboy_cleanup(struct gboy *gboy);
void    gboy_run(struct gboy *gb, const char *path);

#endif
