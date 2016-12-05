#ifndef OWN_SDL_H
#define OWN_SDL_H

#include <SDL/SDL.h>

void wait_for_keypressed(void);

void putpixel(SDL_Surface *surface, unsigned x, unsigned y, Uint32 pixel);

Uint32 getpixel(SDL_Surface *surface, unsigned x, unsigned y);

void init_sdl(void);

SDL_Surface* load_image(char *path);

void display_image(SDL_Surface *img);

void tograyscale(SDL_Surface* img);

void tobinary(SDL_Surface* img, int threshold);

#endif
