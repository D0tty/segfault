#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <err.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

void wait_for_keypressed(void)
{
  SDL_Event             event;
  for (;;)
  {
    SDL_PollEvent( &event );
    switch (event.type) {
      case SDL_KEYDOWN: return;
      default: break;
    }
  }
}

/* SDL */

static inline
Uint8* pixelref(SDL_Surface *surf, unsigned x, unsigned y)
{
  int bpp = surf->format->BytesPerPixel;
  return (Uint8*)surf->pixels + y * surf->pitch + x * bpp;
}

void putpixel(SDL_Surface *surface, unsigned x, unsigned y, Uint32 pixel)
{
  Uint8 *p = pixelref(surface, x, y);
  switch(surface->format->BytesPerPixel)
  {
    case 1:
      *p = pixel;
      break;
    case 2:
      *(Uint16 *)p = pixel;
      break;
    case 3:
      if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
      {
        p[0] = (pixel >> 16) & 0xff;
        p[1] = (pixel >> 8) & 0xff;
        p[2] = pixel & 0xff;
      }
      else
      {
        p[0] = pixel & 0xff;
        p[1] = (pixel >> 8) & 0xff;
        p[2] = (pixel >> 16) & 0xff;
      }
      break;
    case 4:
      *(Uint32 *)p = pixel;
      break;
  }
}

Uint32 getpixel(SDL_Surface *surface, unsigned x, unsigned y)
{
  Uint8 *p = pixelref(surface, x, y);
  switch(surface->format->BytesPerPixel)
  {
    case 1:
      return *p;
    case 2:
      return *(Uint16 *)p;
    case 3:
      if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
        return p[0] << 16 | p[1] << 8 | p[2];
      else
        return p[0] | p[1] << 8 | p[2] << 16;
    case 4:
      return *(Uint32 *)p;
  }
  return 0;
}

void init_sdl(void)
{
  if( SDL_Init(SDL_INIT_VIDEO)==-1 )
  {
    errx(1,"Could not initialize SDL: %s.\n", SDL_GetError());
  }
}

SDL_Surface* load_image(char *path)
{
  SDL_Surface          *img;
  img = IMG_Load(path);
  if (!img)
    errx(3, "can't load %s: %s", path, IMG_GetError());
  return img;
}

void display_image(SDL_Surface *img)
{
  SDL_Surface          *screen;
  screen = SDL_SetVideoMode(img->w, img->h, 0, SDL_SWSURFACE|SDL_ANYFORMAT);
  if ( screen == NULL )
  {
    errx(1, "Couldn't set %dx%d video mode: %s\n",
        img->w, img->h, SDL_GetError());
  }

  /* Blit onto the screen surface */
  if(SDL_BlitSurface(img, NULL, screen, NULL) < 0)
    warnx("BlitSurface error: %s\n", SDL_GetError());

  SDL_UpdateRect(screen, 0, 0, img->w, img->h);

  wait_for_keypressed();

  SDL_FreeSurface(screen);
}

void tograyscale(SDL_Surface* img)
{
  for ( int x = 0; x < img->w; ++x )
  {
    for ( int y = 0; y < img->h; ++y )
    {
      Uint8 R, G, B;
      SDL_GetRGB(getpixel(img, x, y), img->format, &R, &G, &B);
      float luminance = 0.3 * R + 0.59 * G + 0.11 * B;
      R = G = B = luminance;
      Uint32 pix = SDL_MapRGB(img->format, R, G, B);
      putpixel(img,x,y,pix);
    }
  }
}

void tobinary(SDL_Surface* img, int threshold)
{
  for ( int x = 0; x < img->w; ++x )
  {
    for ( int y = 0; y < img->h; ++y )
    {
      Uint8 R, G, B;
      SDL_GetRGB(getpixel(img, x, y), img->format, &R, &G, &B);
      R = G = B = (R+G+B)/3 < (Uint8)threshold ? 0 : 255;
      Uint32 pix = SDL_MapRGB(img->format, R, G, B);
      putpixel(img,x,y,pix);
    }
  }
}
