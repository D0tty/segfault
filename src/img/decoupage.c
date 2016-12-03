#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <err.h>
#include <string.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include "img.h"
#include "decoupage.h"

#define TAILLE 20

struct line* line_create(struct image *chr)
{
  struct line *ln;
  ln = malloc(sizeof(struct line));
  ln->current_char = chr;
  ln->next_char = NULL;
  return ln;
}

void line_free(struct line *ln)
{
  struct line *tmp = ln;
  while(tmp != NULL)
  {
    tmp = ln->next_char;
    image_free(ln->current_char);
    free(ln);
    ln = tmp;
  }
}

struct paragraph* paragraph_create(struct line *ln)
{
  struct paragraph *prgph;
  prgph = malloc(sizeof(struct paragraph));
  prgph->current_line = ln;
  prgph->next_line = NULL;
  return prgph;
}

void paragraph_free(struct paragraph *pa)
{
  struct paragraph *tmp = pa;
  while(pa != NULL)
  {
    tmp = pa->next_line;
    line_free(pa->current_line);
    free(pa);
    pa = tmp;
  }
}

struct page* page_create(struct paragraph *prgph)
{
  struct page *pg;
  pg = malloc(sizeof(struct page));
  pg->current_paragraph = prgph;
  pg->next_paragraph = NULL;
  return pg;
}

void page_free(struct page *pg)
{
  struct page *tmp = pg;
  while(pg != NULL)
  {
    tmp = pg->next_paragraph;
    paragraph_free(pg->current_paragraph);
    free(pg);
    pg = tmp;
  }
}

/*
 * count if bool (0,1)
 * if count, compute the sum of needed chars
 * else return the value
 */
size_t paragraph_compt(struct page *pg, int count)
{
  static size_t compt = 0;
  if ( count )
  {
    while(pg != NULL)
    {
      compt = compt + line_compt(pg->current_paragraph);
      pg = pg->next_paragraph;
      ++compt;
    }
  }
  return compt;
}

size_t line_compt(struct paragraph *prgh)
{
  size_t compt = 0;
  while(prgh != NULL)
  {
    compt = compt + char_compt(prgh->current_line);
    prgh = prgh->next_line;
    ++compt;
  }
  return compt;
}

size_t char_compt(struct line *ln)
{
  size_t compt = 0;
  while(ln != NULL)
  {
    ln = ln->next_char;
    ++compt;
  }
  return compt;
}

struct page* image_to_page(struct image *img)
{
  struct image *ln = first_line_in_paragraph(img);
  int lh = ln->h;
  struct page *pg = NULL;
  pg = to_page(img, pg, lh);
  image_free(ln);
  return pg;
}

struct page* to_page(struct image *img, struct page *pg, int lh)
{
  if(img->h == 0)
  {
    return NULL;
  }
  else
  {
    img = image_get_paragraph(img);
    struct image *prgph = first_paragraph_in_page(img, lh);
    if(prgph->h == img->h && prgph->w == img->w)
    {
      img = image_create(0,0);
    }
    else
    {
      img = image_get_rect(img, 0, prgph->h, img->w - 1, img->h - 1);
      img = image_get_paragraph(img);
    }

    struct paragraph *prg = NULL;
    prg = to_paragraph(prgph, prg);

    pg = page_create(prg);

    pg->next_paragraph = to_page(img, pg->next_paragraph, lh);

    image_free(prgph);
    return pg;
  }
}


struct paragraph* to_paragraph(struct image *img, struct paragraph *prgph)
{
  if(img->h == 0)
  {
    return NULL;
  }
  else
  {
    img = image_get_paragraph(img);
    struct image *ligne = first_line_in_paragraph(img);
    ligne = lateral_cut(ligne);

    img = image_get_rect(img, 0, ligne->h, img->w - 1, img->h - 1);
    img = image_get_paragraph(img);
    struct image *space = first_char_in_line(ligne);
    int l = (space->w / 6) * 4;

    struct line *ln = NULL;
    ln = to_line(ligne, ln, l);
    prgph = paragraph_create(ln);

    prgph->next_line = to_paragraph(img, prgph->next_line);

    image_free(ligne);
    image_free(space);
    return prgph;
  }

}

struct line* to_line(struct image *img, struct line *ligne, int l)
{
  if(img->w == 0)
  {
    return NULL;
  }
  else
  {
    img = lateral_cut(img);
    struct image *chr = first_char_in_line(img);
    img = image_get_rect(img, chr->w, 0, img->w - 1, img->h - 1);
    chr = resizing(image_to_rect(chr), TAILLE);
    if(is_space(img, l) == 1)
    {
      ligne = line_create(chr);
      struct image *esp = image_get_rect(img, 0, 0, l, img->h - 1);
      esp = resizing(image_to_rect(esp), TAILLE);
      ligne->next_char = line_create(esp);
      img = lateral_cut(img);
      ligne->next_char->next_char = to_line(img,ligne->next_char->next_char,l);
    }
    else
    {
      img = lateral_cut(img);
      ligne = line_create(chr);
      ligne->next_char = to_line(img, ligne->next_char, l);
    }
    return ligne;
  }
}

int is_space(struct image *img, int le)
{
  int x = 0,
      w = img->w;
  while(x < w && is_column_blank(img, x))
  {
    ++x;
  }
  return (x >= le) ? 1 : 0;
}

struct image* first_paragraph_in_page(struct image *img, int lh)
{
  int i = 0, y = 0, h = img->h;
  while(y < h)
  {
    while(!is_line_blank(img, y) && y < h)
    {
      ++y;
    }
    while(is_line_blank(img, y) && y < h)
    {
      ++y;
      ++i;
    }
    if(i > lh)
    {
      return image_get_rect(img, 0, 0, img->w - 1, y - 1);
    }
    i = 0;
  }
  return img;
}

struct image* first_char_in_line(struct image *img)
{
  int x = 0, w = img->w;
  while(x < w && !(is_column_blank(img, x)))
  {
    ++x;
  }
  return image_get_rect(img, 0, 0, x - 1, img->h - 1);
}

struct image* first_line_in_paragraph(struct image *img)
{
  int h = img->h, y = 0;
  while(y < h && !is_line_blank(img, y))
  {
    ++y;
  }
  return image_get_rect(img, 0, 0, img->w - 1, y - 1);
}

struct image* image_to_rect(struct image *img)
{
  if(!space(img))
  {
    img = image_get_paragraph(img);
  }
  int w = img->w,
      h = img->h,
      dif = w - h;
  if(dif > 0)
  {
    img = add_height(img, dif);
  }
  else if(dif < 0)
  {
    img = add_width(img, -1 * dif);
  }
  return img;
}

struct image* add_height(struct image *img, int dif)
{
  int notEven = (dif % 2 == 0) ? 0 : 1;
  dif = dif/2;
  struct image *haut = image_create(img->w, dif),
               *bas = image_create(img->w, dif + notEven);
  img = image_merge_vertical(haut, img);
  img = image_merge_vertical(img, bas);
  return img;
}

struct image* add_width(struct image *img, int dif)
{
  int notEven = (dif % 2 == 0) ? 0 : 1;
  dif = dif/2;
  struct image *left = image_create(dif, img->h),
               *right = image_create(dif + notEven, img->h);
  img = image_merge(left, img);
  img = image_merge(img, right);
  return img;
}

int space(struct image *img)
{
  int w = img->w,
      i = 0;
  while(i < w && is_column_blank(img, i))
  {
    ++i;
  }
  return (i == w) ? 1 : 0;
}

struct page* get_page(struct image *img)
{
  struct image *i = first_line_in_paragraph(img);
  int lh = i->h;
  image_free(i);

  struct page *pg = NULL;

  pg = to_page(img, pg, lh);
  return pg;
}
