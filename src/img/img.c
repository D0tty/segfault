#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <err.h>
#include <string.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include "img.h"
#include "decoupage.h"

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
  Uint8 threshold = 150;
  for ( int x = 0; x < img->w; ++x )
  {
    for ( int y = 0; y < img->h; ++y )
    {
      Uint8 R, G, B;
      SDL_GetRGB(getpixel(img, x, y), img->format, &R, &G, &B);
      R = G = B = (R+G+B)/3 < threshold ? 0 : 255;
      Uint32 pix = SDL_MapRGB(img->format, R, G, B);
      putpixel(img,x,y,pix);
    }
  }
  return img;
}

SDL_Surface* display_image(SDL_Surface *img)
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

  return screen;
}

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
      int val = image_pixel(img, i, j);
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
      int val = image_pixel(img, i, j);
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
  struct image *img = image_create(sdlimg->w, sdlimg->h);
  for(int j = 0; j < img->h; ++j)
  {
    for(int i = 0; i < img->w; ++i)
    {
      Uint32 pix = getpixel(sdlimg, i, j);
      image_pixel(img, i, j) = pix != 0;
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

struct image* image_get_paragraph(struct image *img)
{
  int x = 0,
      y = 0,
      z = img->w - 1,
      t = img->h - 1;
  if(t == -1 || z == -1)
    return image_create(0,0);
  while(is_column_blank(img, x) && x <= z)
  {
    ++x;
  }
  if(x == z)
    return image_create(0,0);
  while(is_column_blank(img, z) && z >= 0)
  {
    --z;
  }
  while(is_line_blank(img, y) && y <= t)
  {
    ++y;
  }
  if(y == t)
    return image_create(0,0);
  while(is_line_blank(img, t) && t >= 0)
  {
    --t;
  }
  return image_get_rect(img, x, y, z, t);
}

struct image* image_merge(struct image *img1, struct image *img2)
{
  struct image *img = image_create(img1->w + img2->w, img1->h);
  int i = 0;
  for(i = 0; i < img->w; ++i)
  {
    for(int j = 0; j < img->h; ++j)
      image_pixel(img, i + img1->w, j) = image_pixel(img2, i, j);
  }
  for(i = 0; i < img1->w; ++i)
  {
    for(int j = 0; j < img->h; ++j)
      image_pixel(img, i, j) = image_pixel(img1, i, j);
  }
  return img;
}

struct image* line_to_image(struct line *ln)
{
  struct line *tmp = ln->next_char;
  struct image *img = image_get_rect(ln->current_char, 0, 0,\
      ln->current_char->w - 1, ln->current_char->h - 1);
  while(tmp != NULL)
  {
    img = image_merge(img,tmp->current_char);
    tmp = tmp->next_char;
  }
  return img;
}


int main(int argc, char *argv[])
{
  if(argc < 2)
    return 1;
  //SDL init, load, gray, B&W
  init_sdl();
  SDL_Surface* sdlimg = load_image(argv[1]);
  tograyscale(sdlimg);
  tobinary(sdlimg);

  //create struct image
  struct image *img = image_get_from_SDL(sdlimg);

  //printf("\n");
  //img = image_get_paragraph(img);
  //sdlimg = to_sdl_image(img);
  struct image *i = first_line_in_paragraph(img);
  int lh = i->h;

  struct page *pg = NULL;
  pg = to_page(img, pg, lh);
  while(pg != NULL)
  {
    struct paragraph *prg = pg->current_paragraph;
    while(prg != NULL)
    {
      struct line *lg = prg->current_line;
      while(lg != NULL)
      {
        display_image(to_sdl_image(lg->current_char));
        lg = lg->next_char;
      }
      prg = prg->next_line;
    }
    pg = pg->next_paragraph;
  }

  struct line *ln = line_create(img);
  ln->next_char = line_create(img);
  struct image *test = line_to_image(ln);
  image_prety_print(test);

  image_free(i);
  //page_free(pg);
  //struct line *ln = line_create(img);
  //line_free(ln);

  /* free  */
  SDL_FreeSurface(sdlimg);
  //SDL_FreeSurface(s);
  // if you try to free img after free ln which contains img in segfaults
  // image_free(img);
  SDL_Quit();

  return 0;
}
