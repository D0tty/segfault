#ifndef DECOUPAGE_H
#define DECOUPAGE_H

struct line
{
  struct image *current_char;
  struct line *next_char;
};

struct paragraph
{
  struct line *current_line;
  struct paragraph *next_line;
};

struct line* line_create(struct image *chr);

void line_free(struct line *ln);

struct paragraph* paragraph_create(struct line *ln);

void paragraph_free(struct paragraph *pa);

struct line* ligne_to_line(struct image *img, struct line *ligne);

struct image* first_char_in_line(struct image *img);

int next_black_line(struct image *img, int y);

#endif
