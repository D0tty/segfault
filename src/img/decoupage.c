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

/*POISSON FREE() POISSON FREE()*/



//A refaire récursivement demain (sale sale faut pas regarder)

/*struct paragraph* decoupage(struct image *img)
{
  img = image_get_paragraph(img);
  struct paragraph *prgph;
  int y1 = 0, y2 = 0, cp = 0, w = img->w, h = img->h;
  while(y2 < h)
  {
    if(is_line_blank(img, y2))
    {
      struct image *lgn = image_get_rect(img, 0, y1, w-1, y2);
      // FAIRE LE TRAITEMENT DE LA LIGNE TROUVEE ICI
      y2 = next_black_line(img, y2);
      y1 = y2;
    }
    else
    {
      ++y2
    }
  }
}*/

struct line* ligne_to_line(struct image *img, struct line *ligne)
{
  //Verifier si un image_get_rect(img, 0, 0, 0, 0) est NULL ou a
  //Une hauteur et une largeur de 0 et adapter en fonction
  if(img->w == 0)
  {
    return NULL;
  }
  else
  {
  struct image *chr = first_char_in_line(img);

  img = image_get_rect(img, chr->w, 0, img->w - 1, img->h - 1);
  img = image_get_paragraph(img);
  /*IMPLEMENTER FONCTION IS_ESPACE => si blanc >= largeur char => espace*/
  ligne = line_create(chr);
  ligne->next_char = ligne_to_line(img, ligne->next_char);
  return ligne;
  }
}

struct image* first_char_in_line(struct image *img)
{
  int x = 0, w = img->w;
  while(x < w && !(is_column_blank(img, x)))
  {
    ++x;
  }
  return image_get_rect(img, 0, 0, x, img->h - 1);
}

//va surement disparaitre demain avec le passage a la recursion
int next_black_line(struct image *img, int y)
{
  int h = img->h;
  while(y < h && is_line_blank(img, y))
  {
    ++y;
  }
  return y;
}
