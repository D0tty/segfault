#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <err.h>
#include <string.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include "img.h"
#include "decoupage.h"

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

struct page* image_to_page(struct image *img)
{
  struct image *ln = first_line_in_paragraph(img);
  int lh = ln->h;
  struct page *pg = NULL;
  pg = to_page(img, pg, lh);
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

    img = image_get_rect(img, 0, prgph->h, img->w - 1, img->h - 1);
    img = image_get_paragraph(img);

    struct paragraph *prg = NULL;
    prg = to_paragraph(prgph, prg);

    pg = page_create(prg);

    pg->next_paragraph = to_page(img, pg->next_paragraph, lh);
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

    img = image_get_rect(img, 0, ligne->h, img->w - 1, img->h - 1);
    img = image_get_paragraph(img);

    struct line *ln = NULL;
    ln = to_line(ligne, ln);
    prgph = paragraph_create(ln);

    prgph->next_line = to_paragraph(img, prgph->next_line);
    return prgph;
  }

}

struct line* to_line(struct image *img, struct line *ligne)
{
  //Verifier si un image_get_rect(img, 0, 0, 0, 0) est NULL ou a
  //Une hauteur et une largeur de 0 et adapter en fonction
  if(img->w == 0)
  {
    return NULL;
  }
  else
  {
    img = image_get_paragraph(img);
    struct image *chr = first_char_in_line(img);
    img = image_get_rect(img, chr->w, 0, img->w - 1, img->h - 1);
    img = image_get_paragraph(img);
    /*IMPLEMENTER FONCTION IS_ESPACE => si blanc >= largeur char => espace*/
    ligne = line_create(chr);
    ligne->next_char = to_line(img, ligne->next_char);
    return ligne;
  }
}

struct image* first_paragraph_in_page(struct image *img, int lh)
{
  int i = 0, y = 0, h = img->h;
  while(y < h)
  {
    while(!is_line_blank(img, y))
    {
      ++y;
    }
   while(is_line_blank(img, y))
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
