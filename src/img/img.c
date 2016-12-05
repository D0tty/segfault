#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <err.h>
#include <string.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <math.h>
#include "img.h"
#include "decoupage.h"
#include "../v2/sdl.h"

#define THRESHOLD 130
#define PI 3.14159


SDL_Surface* noise_reduction(SDL_Surface *img)
{
  for(int j = 1; j < img->w - 1; ++j)
  {
    for(int i = 1; i < img->h - 1; ++i)
    {
      Uint8 R, G, B;
      int RR = 0, GG = 0, BB = 0;
      for(int x = -1; x <= 1; ++x)
      {
        for(int y = -1; y <= 1; ++y)
        {
          SDL_GetRGB(getpixel(img, j + x, i + y), img->format, &R, &G, &B);
          RR += R;
          GG += G;
          BB += B;
        }
      }
      RR = RR / 9;
      GG = GG / 9;
      BB = BB / 9;
      Uint32 pix = SDL_MapRGB(img->format, RR, GG, BB);
      putpixel(img,j,i,pix);
    }
  }
  return img;
}

SDL_Surface* right_rotation(SDL_Surface *img)
{
  SDL_Surface *new = SDL_CreateRGBSurface(0, img->h, img->w, 32, 0, 0, 0, 0);
  for(int j = 0; j < img->w; ++j)
  {
    for(int i = 0; i < img->h; ++i)
    {
      putpixel(new, img->h - 1 - i, j, getpixel(img, j, i));
    }
  }
  return new;
}

SDL_Surface* left_rotation(SDL_Surface *img)
{
  SDL_Surface *new = SDL_CreateRGBSurface(0, img->h, img->w, 32, 0, 0, 0, 0);
  for(int j = 0; j < img->w; ++j)
  {
    for(int i = 0; i < img->h; ++i)
    {
      putpixel(new, i, img->w - 1 - j, getpixel(img, j, i));
    }
  }
  return new;
}

SDL_Surface* rotation(SDL_Surface *img, double angle)
{
  angle = angle * PI/180;
  SDL_Surface *new = SDL_CreateRGBSurface(0, img->w, img->h, 32, 0, 0, 0, 0);
  double cs = cos(angle),
         sn = sin(angle);
  int x = 0,
      y = 0,
      it = 0,
      jt = 0,
      hw = img->w / 2,
      hh = img->h / 2;
  for(int a = 0; a < img->w; ++a)
  {
    for(int b = 0; b < img->h; ++b)
    {
      putpixel(new, a, b, SDL_MapRGB(img->format, 255, 255, 255));
    }
  }
  for(int j = 0; j < img->w; ++j)
  {
    for(int i = 0; i < img->h; ++i)
    {
      jt = j - hw;
      it = i - hh;
      x = jt * cs - it * sn + hw;
      y = it * cs + jt * sn + hh;
      if(x >= 0 && y >= 0 && x < img->w && y < img->h)
      {
        putpixel(new, x, y, getpixel(img, j, i));
      }
    }
  }
  return new;
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
      image_pixel(img, i, j) = 1;
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

int is_img_blank(struct image *img)
{
  int w = img->w, x = 0;
  int is_blank = 1;
  while ( is_blank && x < w )
  {
    is_blank = is_column_blank(img,x);
    ++x;
  }
  return is_blank;
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

struct image* lateral_cut(struct image *img)
{
  if(img->w == 0 || img->h == 0)
  {
    return image_create(0,0);
  }
  int x = 0, w = img->w - 1;
  while(x <= w && is_column_blank(img, x))
  {
    ++x;
  }
  if(x == w)
    return image_create(0,0);
  while(w > 0 && is_column_blank(img, w))
  {
    ++w;
  }
  return image_get_rect(img, x, 0, w, img->h - 1);
}

struct image* image_merge(struct image *img1, struct image *img2)
{
  struct image *img = image_create(img1->w + img2->w, img1->h);
  int i = 0;
  //copy img1 to img
  for(i = 0; i < img1->w; ++i)
  {
    for(int j = 0; j < img->h; ++j)
      image_pixel(img, i, j) = image_pixel(img1, i, j);
  }
  //copy img2 to img with an offset on the X axis: img->w
  for(i = 0; i < img2->w; ++i)
  {
    for(int j = 0; j < img->h; ++j)
      image_pixel(img, i + img1->w, j) = image_pixel(img2, i, j);
  }
  return img;
}

struct image* image_merge_vertical(struct image *img1, struct image *img2)
{
  int mx = (img1->w > img2->w) ? img1->w : img2->w;
  struct image *img = image_create(mx, img1->h + img2->h);
  int i = 0;

  for(i = 0; i < img1->w; ++i)
  {
    for(int j = 0; j < img1->h; ++j)
    {
      image_pixel(img, i, j) = image_pixel(img1, i, j);
    }
  }
  for(i = 0; i < img2->w; ++i)
  {
    for(int j = 0; j < img2->h; ++j)
    {
      image_pixel(img, i, j + img1->h) = image_pixel(img2, i, j);
    }
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

struct image* paragraph_to_image(struct paragraph *prg)
{
  struct paragraph *tmp = prg->next_line;
  struct image *img = line_to_image(prg->current_line);
  while(tmp != NULL)
  {
    img = image_merge_vertical(img, line_to_image(tmp->current_line));
    tmp = tmp->next_line;
  }
  return img;
}

struct image* resizing(struct image *img, int nt)
{
  int t = img->w;
  struct image *resized = image_create(nt, nt);
  for(int i = 0; i < nt; ++i)
  {
    for(int j = 0; j < nt; ++j)
    {
      image_pixel(resized, i, j) = image_pixel(img, (i*t)/nt, (j*t)/nt);
    }
  }
  return resized;
}

int test_main(int argc, char *argv[])
{
  if(argc < 2)
    return 1;
  double e = 90;
  //SDL init, load, gray, B&W
  init_sdl();
  SDL_Surface* sdlimg = load_image(argv[1]);

  //dispaly color
  display_image(sdlimg);
  display_image(rotation(sdlimg, e));

  //display gray
  tograyscale(sdlimg);
  display_image(sdlimg);

  //display B&W
  tobinary(sdlimg,130);
  display_image(sdlimg);

  //create struct image
  struct image *img = image_get_from_SDL(sdlimg);

  //display paragraph
  img = image_get_paragraph(img);

  sdlimg = to_sdl_image(img);
  display_image(sdlimg);



  struct image *i = first_line_in_paragraph(img);
  int lh = i->h;

  struct image *laligne;
  SDL_Surface* sdllaligne;


  struct page *pg = NULL;
  pg = to_page(img, pg, lh);
  while(pg != NULL)
  {
    struct paragraph *prg = pg->current_paragraph;
    display_image(to_sdl_image(paragraph_to_image(prg)));
    while(prg != NULL)
    {
      struct line *lg = prg->current_line;
      laligne = line_to_image(lg);
      sdllaligne = to_sdl_image(laligne);
      display_image(sdllaligne);
      while(lg != NULL)
      {
        display_image(to_sdl_image(lg->current_char));
        lg = lg->next_char;
      }
      prg = prg->next_line;
    }
    pg = pg->next_paragraph;
  }

  image_free(i);
  page_free(pg);
  image_free(laligne);

  /* free  */
  SDL_FreeSurface(sdlimg);
  SDL_FreeSurface(sdllaligne);
  // if you try to free img after free ln which contains img in segfaults
  // image_free(img);
  SDL_Quit();

  return 0;
}
