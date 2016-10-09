#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <err.h>
#include <string.h>
#include <SDL.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include "img.h"


/* SDL */

static inline
Uint8* pixelref(SDL_Surface *surf, unsigned x, unsigned y) {
  int bpp = surf->format->BytesPerPixel;
  return (Uint8*)surf->pixels + y * surf->pitch + x * bpp;
}

void putpixel(SDL_Surface *surface, unsigned x, unsigned y, Uint32 pixel) {
  Uint8 *p = pixelref(surface, x, y);
  switch(surface->format->BytesPerPixel) {
    case 1:
      *p = pixel;
      break;
    case 2:
      *(Uint16 *)p = pixel;
      break;
    case 3:
      if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
        p[0] = (pixel >> 16) & 0xff;
        p[1] = (pixel >> 8) & 0xff;
        p[2] = pixel & 0xff;
      } else {
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

Uint32 getpixel(SDL_Surface *surface, unsigned x, unsigned y) {
  Uint8 *p = pixelref(surface, x, y);
  switch(surface->format->BytesPerPixel) {
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
void init_sdl(void) {
  // Init only the video part
  if( SDL_Init(SDL_INIT_VIDEO)==-1 ) {
    // If it fails, die with an error message
    errx(1,"Could not initialize SDL: %s.\n", SDL_GetError());
  }
  // We don't really need a function for that ...
}

SDL_Surface* load_image(char *path) {
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

/* SDL */



struct image* image_create(int w, int h)
{
  struct image *img;
  img = malloc ( sizeof( struct image) ) ;
  img->w = w;
  img->h = h;
  img->data = malloc ( sizeof(int) * (w * h));
  for(int i = 0; i < w; ++i)
  {
    for(int j = 0; j < h; ++j)
    {
      img->data[img->w * j + i] = 0;
    }
  }
  return img;
}

void image_fill(struct image *img)
{
  int k = -1;
  for(int i = 0; i < img->w; ++i)
  {
    for(int j = 0; j < img->h; ++j)
    {
      img->data[img->w * j + i] = ++k;
    }
  }
}

void image_print(struct image *img)
{
  for(int i = 0; i < img->w; ++i)
  {
    for(int j = 0; j < img->h; ++j)
    {
      int val = img->data[img->w * j + i];
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
      int val = img->data[img->w * j + i];
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
  for(int i = 0; i < img->w; ++i)
  {
    for(int j = 0; j < img->h; ++j)
    {
      img->data[img->w * j + i] = getpixel(sdlimg, i, j) != 0;
    }
  }
  return img;
}

void image_free(struct image *img)
{
  free(img->data);
  free(img);
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

  SDL_FreeSurface(sdlimg);
  image_free(img);

  return 0;
}
