#include "screen.h"

void screen_init(struct screen *screen) {
    screen->window = SDL_CreateWindow("gboy", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        800, 600, SDL_WINDOW_SHOWN);
    screen->screen = SDL_GetWindowSurface(screen->window);
    screen->buffer = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32, 0, 0, 0, 0);
    memset(screen->back_buffer, 0xFF, sizeof(screen->back_buffer));
}

void screen_cleanup(struct screen *screen) {
    SDL_FreeSurface(screen->buffer);
    SDL_DestroyWindow(screen->window);
}

void screen_update(struct screen *screen) {
    SDL_LockSurface(screen->buffer);
    memcpy(screen->buffer->pixels, screen->back_buffer, sizeof(screen->back_buffer));
    SDL_UnlockSurface(screen->buffer);
    SDL_BlitScaled(screen->buffer, NULL, screen->screen, NULL);
    SDL_UpdateWindowSurface(screen->window);
}
