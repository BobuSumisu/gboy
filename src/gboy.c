#include <stdlib.h>
#include <SDL2/SDL.h>
#include <time.h>
#include "gboy.h"
#include "cartridge.h"

/* TODO: move a lot of this into cpu. */

/*
 *  Timing
 *  ======
 *
 *  60 FPS = 0.01667s/frame = 16.67 ms/frame
 *  Game Boy run at 4194304 Hz = ~69905 cycles/frame (t-cycles)
 *
 */

const float CYCLES_PER_SEC      = 4194304.0;
const float FRAMES_PER_SEC      = 60.0f;
const float MS_PER_FRAME        = (1000.0f / (float)FRAMES_PER_SEC);
const float CYCLES_PER_FRAME    = (CYCLES_PER_SEC / FRAMES_PER_SEC);

/*** Private ***/

static void gboy_handle_sdl_events(struct gboy *gb) {
    SDL_Event evt;
    while(SDL_PollEvent(&evt)) {
        switch(evt.type) {
            case SDL_QUIT:
                gb->cpu.running = 0;
                break;
            case SDL_KEYDOWN:
                switch(evt.key.keysym.sym) {
                    case SDLK_ESCAPE:
                    case SDLK_q:
                        gb->cpu.running = 0;
                        break;
                    case SDLK_d:
                        gb->debug = 1;
                        break;
                    case SDLK_p:
                        cpu_debug(&gb->cpu);
                        break;
                    case SDLK_RIGHT: input_keydown(&gb->input, KEY_RIGHT); break;
                    case SDLK_LEFT: input_keydown(&gb->input, KEY_LEFT); break;
                    case SDLK_UP: input_keydown(&gb->input, KEY_UP); break;
                    case SDLK_DOWN: input_keydown(&gb->input, KEY_DOWN); break;
                    case SDLK_z: input_keydown(&gb->input, KEY_A); break;
                    case SDLK_c: input_keydown(&gb->input, KEY_B); break;
                    case SDLK_SPACE: input_keydown(&gb->input, KEY_SELECT); break;
                    case SDLK_RETURN: input_keydown(&gb->input, KEY_START); break;
                }
                break;
            case SDL_KEYUP:
                switch(evt.key.keysym.sym) {
                    case SDLK_RIGHT: input_keyup(&gb->input, KEY_RIGHT); break;
                    case SDLK_LEFT: input_keyup(&gb->input, KEY_LEFT); break;
                    case SDLK_UP: input_keyup(&gb->input, KEY_UP); break;
                    case SDLK_DOWN: input_keyup(&gb->input, KEY_DOWN); break;
                    case SDLK_z: input_keyup(&gb->input, KEY_A); break;
                    case SDLK_c: input_keyup(&gb->input, KEY_B); break;
                    case SDLK_SPACE: input_keyup(&gb->input, KEY_SELECT); break;
                    case SDLK_RETURN: input_keyup(&gb->input, KEY_START); break;
                }
                break;
        }
    }
}

/*** Public ***/

int gboy_init(struct gboy *gb) {
    memset(gb, 0, sizeof(struct gboy));
    gb->debug = 0;

    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
        fprintf(stderr, "failed to initialize SDL: %s\n", SDL_GetError());
        return -1;
    }

    cpu_init(&gb->cpu, &gb->mmu, &gb->ic);
    interrupt_controller_init(&gb->ic, &gb->cpu);
    mmu_init(&gb->mmu, &gb->cpu, &gb->ic, &gb->gpu, &gb->timer, &gb->input, &gb->apu);
    screen_init(&gb->screen);
    gpu_init(&gb->gpu, &gb->ic, &gb->screen);
    timer_init(&gb->timer, &gb->ic);
    input_init(&gb->input);
    apu_init(&gb->apu);

    /* Skip boot. */
    if(1) {
        gb->cpu.af = 0x01B0;
        gb->cpu.bc = 0x0013;
        gb->cpu.de = 0x00D8;
        gb->cpu.hl = 0x014D;
        gb->cpu.pc = 0x0100;
        gb->cpu.sp = 0xFFFE;
        gb->mmu.reg_boot = 1;

        gpu_io_set_lcdc(&gb->gpu, 0x91);
        gpu_io_set_bgp(&gb->gpu, 0xFC);
        gpu_io_set_obp0(&gb->gpu, 0xFF);
        gpu_io_set_obp1(&gb->gpu, 0xFF);

        mmu_wb(&gb->mmu, 0xFF02, 0x7E);
    }

    return 0;
}

void gboy_cleanup(struct gboy *gb) {
    cpu_cleanup(&gb->cpu);
    interrupt_controller_cleanup(&gb->ic);
    mmu_cleanup(&gb->mmu);
    screen_cleanup(&gb->screen);
    gpu_cleanup(&gb->gpu);
    timer_cleanup(&gb->timer);
    input_cleanup(&gb->input);
    apu_cleanup(&gb->apu);
    SDL_Quit();
}

void gboy_run(struct gboy *gb, const char *path) {

    struct cartridge cartridge;
    if(cartridge_open(&cartridge, path) != 0) {
        fprintf(stderr, "failed to open cartridge: %s\n", path);
        return;
    }

    mmu_load_rom(&gb->mmu, path);

    uint32_t time = SDL_GetTicks();
    uint16_t cycles = 0;
    uint64_t total_cycles = 0;

    gb->cpu.running = 1;

    while(gb->cpu.running) {
        cycles = cpu_step(&gb->cpu);
        total_cycles += cycles;

        interrupt_controller_handle(&gb->ic);
        timer_update(&gb->timer, cycles);
        gpu_update(&gb->gpu, cycles);
        apu_update(&gb->apu, cycles);

        if(gb->debug) {
            cpu_debug(&gb->cpu);
            printf("LCDC: 0x%02X STAT: 0x%02X, LY: 0x%02X\n", gb->gpu.reg_lcdc,
                gb->gpu.reg_stat, gb->gpu.reg_ly);
            if(getchar() == -1) {
                gb->debug = 0;
            }
        }

        /* Update screen if a frame has passed. */
        if(total_cycles > CYCLES_PER_FRAME) {
            total_cycles %= (int)CYCLES_PER_FRAME;
            screen_update(&gb->screen);
            gboy_handle_sdl_events(gb);

            time = SDL_GetTicks() - time;
            if(time < MS_PER_FRAME) {
                SDL_Delay((uint32_t)(MS_PER_FRAME - time));
            }
            time = SDL_GetTicks();
        }
    }

    // getchar();
}
