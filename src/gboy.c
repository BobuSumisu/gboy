#include <stdlib.h>
#include <SDL2/SDL.h>
#include <time.h>
#include "gboy.h"
#include "cpu.h"
#include "mmu.h"
#include "gpu.h"
#include "screen.h"

#define CYCLES_PER_SEC      4194304
#define FRAMES_PER_SEC      60
#define CYCLES_PER_FRAME    (CYCLES_PER_SEC / FRAMES_PER_SEC)

/*** Private ***/

/*** Public ***/

int gboy_init(struct gboy *gb) {
    memset(gb, 0, sizeof(struct gboy));
    gb->debug = 0;

    if(SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "failed to initialize SDL: %s\n", SDL_GetError());
        return -1;
    }

    cpu_init(&gb->cpu, &gb->mmu);
    mmu_init(&gb->mmu, &gb->cpu, &gb->gpu, &gb->timer);
    screen_init(&gb->screen);
    gpu_init(&gb->gpu, &gb->cpu, &gb->screen);
    timer_init(&gb->timer, &gb->cpu);

    /* Skip boot. */
    // gb->mmu.reg_boot = 1;
    // gb->cpu.af = 0x01B0;
    // gb->cpu.bc = 0x0013;
    // gb->cpu.de = 0x00D8;
    // gb->cpu.hl = 0x014D;
    // gb->cpu.pc = 0x0100;
    // gb->cpu.sp = 0xFFFE;
    // gb->gpu.reg_lcdc = 0x91;
    // gb->gpu.reg_bgp = 0xFC;
    // gb->gpu.reg_obp0 = 0xFF;
    // gb->gpu.reg_obp1 = 0xFF;

    return 0;
}

void gboy_cleanup(struct gboy *gb) {
    cpu_cleanup(&gb->cpu);
    mmu_cleanup(&gb->mmu);
    screen_cleanup(&gb->screen);
    gpu_cleanup(&gb->gpu);
    timer_cleanup(&gb->timer);
    SDL_Quit();
}

void gboy_run(struct gboy *gb, const char *path) {
    mmu_load_rom(&gb->mmu, path);
    int total_cycles, cycles;
    SDL_Event evt;

    while(!gb->cpu.halt) {

        total_cycles = 0;
        while(total_cycles < CYCLES_PER_FRAME && !gb->cpu.halt) {
            cycles = cpu_step(&gb->cpu);
            gpu_update(&gb->gpu, cycles);
            timer_update(&gb->timer, cycles);
            total_cycles += cycles;
            if(gb->debug) {
                cpu_debug(&gb->cpu);
                printf("LCDC: 0x%02X LY: 0x%02X\n", gb->gpu.reg_lcdc, gb->gpu.reg_ly);
                if(getchar() == -1) {
                    gb->debug = 0;
                }
            }
        }

        screen_update(&gb->screen);

        while(SDL_PollEvent(&evt)) {
            switch(evt.type) {
                case SDL_QUIT:
                    return;
                case SDL_KEYDOWN:
                    switch(evt.key.keysym.sym) {
                        case SDLK_ESCAPE:
                        case SDLK_q:
                            return;
                        case SDLK_d:
                            gb->debug = 1;
                            break;
                    }
                    break;
            }
        }
    }

    fprintf(stderr, "HALTED [press enter]\n");
    getchar();
}
