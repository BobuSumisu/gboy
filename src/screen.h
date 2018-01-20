/*
 *  screen.h
 *  ========
 *
 *  Emulate a LCD screen by using SDL and blitting to a window.
 *
 */
#ifndef GBOY_SCREEN_H
#define GBOY_SCREEN_H

#include <SDL2/SDL.h>

#define SCREEN_WIDTH    160
#define SCREEN_HEIGHT   144

struct screen {
    SDL_Window *window;
    SDL_Surface *screen;
    SDL_Surface *buffer;
    uint8_t back_buffer[SCREEN_WIDTH * SCREEN_HEIGHT * 4];
};

void    screen_init(struct screen *screen);
void    screen_cleanup(struct screen *screen);
void    screen_update(struct screen *screen);

#endif
