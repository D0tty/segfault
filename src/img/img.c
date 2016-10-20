#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <err.h>
#include <string.h>
#include <SDL.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include "img.h"


void wait_for_keypressed(void)
{
  SDL_Event             event;
   // Infinite loop, waiting for event
  for (;;)
  {
     // Take an event
     SDL_PollEvent( &event );
     // Switch on event type
     switch (event.type) {
     // Someone pressed a key -> leave the function
     case SDL_KEYDOWN: return;
     default: break;
     }
   // Loop until we got the expected event
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
  // Init only the video part
  if( SDL_Init(SDL_INIT_VIDEO)==-1 )
  {
    // If it fails, die with an error message
    errx(1,"Could not initialize SDL: %s.\n", SDL_GetError());
  }
  // We don't really need a function for that ...
}

SDL_Surface* load_image(char *path)
{
  SDL_Surface          *img;
  // Load an image using SDL_image with format detection
  img = IMG_Load(path);
  if (!img)
    // If it fails, die with an error message
    errx(3, "can't load %s: %s", path, IMG_GetError());
  return img;
}

SDL_Surface* tograyscale(SDL_Surface* img)
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
  return img;
}

SDL_Surface* tobinary(SDL_Surface* img)
{
  for ( int x = 0; x < img->w; ++x )
  {
    for ( int y = 0; y < img->h; ++y )
    {
      Uint8 R, G, B;
      SDL_GetRGB(getpixel(img, x, y), img->format, &R, &G, &B);
      R = G = B = (R+G+B)/3 < 60 ? 0 : 255;
      Uint32 pix = SDL_MapRGB(img->format, R, G, B);
      putpixel(img,x,y,pix);
    }
  }
  return img;
}

SDL_Surface* display_image(SDL_Surface *img)
{
  SDL_Surface          *screen;
  // Set the window to the same size as the image
  screen = SDL_SetVideoMode(img->w, img->h, 0, SDL_SWSURFACE|SDL_ANYFORMAT);
  if ( screen == NULL )
  {
   // error management
    errx(1, "Couldn't set %dx%d video mode: %s\n",
    img->w, img->h, SDL_GetError());
  }

  /* Blit onto the screen surface */
  if(SDL_BlitSurface(img, NULL, screen, NULL) < 0)
    warnx("BlitSurface error: %s\n", SDL_GetError());

  // Update the screen
  SDL_UpdateRect(screen, 0, 0, img->w, img->h);

  // wait for a key
  wait_for_keypressed();

  /// return the screen for further uses
  return screen;
}

#define image_pixel(img, i, j) img->data[img->w * (j) + (i)]

SDL_Surface* to_sdl_image(struct image *img)
{
  unsigned w = img->w, h = img->h;
  Uint32 pixel;
  SDL_Surface *surface = SDL_CreateRGBSurface(0, w, h, 32, 0, 0, 0, 0);
  for(unsigned j = 0; j < h; ++j)
  {
    for(unsigned i = 0; i < w; ++i)
    {
      if(image_pixel(img, i, j) == 0)
      {
        pixel = SDL_MapRGB(surface->format, 0, 0, 0);
      }
      else
      {
        pixel = SDL_MapRGB(surface->format, 255, 255, 255);
      }
      putpixel(surface, i, j, pixel);
    }
  }
  return surface;
}

/* SDL */

/* Image Struct Fonctions */

struct image* image_create(int w, int h)
{
  struct image *img;
  img = malloc ( sizeof( struct image) ) ;
  img->w = w;
  img->h = h;
  img->data = malloc ( sizeof(int) * (w * h));
  for(int j = 0; j < h; ++j)
  {
    for(int i = 0; i < w; ++i)
    {
      //img->data[img->w * j + i] = 0;
      image_pixel(img, i, j) = 0;
    }
  }
  return img;
}

void image_fill(struct image *img)
{
  int k = -1;
  for(int j = 0; j < img->h; ++j)
  {
    for(int i = 0; i < img->w; ++i)
    {
      //img->data[img->w * j + i] = ++k;
      image_pixel(img, i, j) = ++k;
    }
  }
}

void image_print(struct image *img)
{
  for(int j = 0; j < img->h; ++j)
  {
    for(int i = 0; i < img->w; ++i)
    {
      int val = image_pixel(img, i, j); //img->data[img->w * j + i];
      printf("| %2d ", val);
    }
    printf("|\n");
  }
}

void image_prety_print(struct image *img)
{
  for(int j = 0; j < img->h; ++j)
  {
    for(int i = 0; i < img->w; ++i)
    {
      int val = image_pixel(img, i, j); //img->data[img->w * j + i];
      //printf("%d",val!=0); //if val == 0 (black) print 0 else print 1 (white)
      if(val!=0)
        printf("1");
      else
        printf(" ");
    }
    printf("\n");
  }
}

struct image* image_get_from_SDL(SDL_Surface* sdlimg)
{
  struct image *img;
  img = image_create(sdlimg->w, sdlimg->h);
  for(int j = 0; j < img->h; ++j)
  {
    for(int i = 0; i < img->w; ++i)
    {
      image_pixel(img, i, j) = getpixel(sdlimg, i, j) != 0;
    }
  }
  return img;
}

struct image* image_get_rect(struct image *img, int x, int y, int z, int t)
{
  int h = (t - y) + 1, w = (z - x) + 1;
  struct image *rectImg = image_create(w, h);
  for(int j = y, jt = 0; j <= t; ++j, ++jt)
  {
    for(int i = x, it = 0; i <= z; ++i, ++it)
    {
      image_pixel(rectImg, it, jt) = image_pixel(img, i, j);
    }
  }
  return rectImg;
}

void image_free(struct image *img)
{
  free(img->data);
  free(img);
}

int is_line_blank(struct image *img, int y)
{
  int x = 0;
  while( x < img->w && image_pixel(img, x, y))
  {
    ++x;
  }
  return x == img->w;
}

int is_column_blank(struct image *img, int x)
{
  int y = 0;
  while( y < img->h && image_pixel(img, x, y))
  {
    ++y;
  }
  return y == img->h;
}

int main(int argc, char *argv[])
{
	if(argc < 2)
    return 1;
  init_sdl();
  SDL_Surface* sdlimg = load_image(argv[1]);
  tograyscale(sdlimg);
  tobinary(sdlimg);

  struct image *img;
  img = image_get_from_SDL(sdlimg);

  image_prety_print(img);

  /*Test de la fonction de découpage en rectangles*/

  printf("\n");
  printf("rectangle autour du texte de coucou_test.jpg");
  printf("\n");
  int x = 4, y = 3, z = img->w -1, t = img->h - 10;
  struct image *rect = image_get_rect(img, x, y, z, t);
  image_prety_print(rect);
  SDL_Surface *i = to_sdl_image(rect);
  display_image(i);

/*
  for(int y = 0; y < img->h; ++y)
  {
    printf("Line %3d, is: %d\n", y, is_line_blank(img, y));
  }

  for(int x = 0; x < img->w; ++x)
  {
    printf("Column %3d, is: %d\n", x, is_column_blank(img, x));
  }*/

  SDL_FreeSurface(sdlimg);
  image_free(img);

  return 0;
}
