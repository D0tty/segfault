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

struct page
{
  struct paragraph *current_paragraph;
  struct page *next_paragraph;
};

struct line* line_create(struct image *chr);

void line_free(struct line *ln);

struct paragraph* paragraph_create(struct line *ln);

void paragraph_free(struct paragraph *pa);

struct page* page_create(struct paragraph *prgph);

void page_free(struct page *pg);

struct page* image_to_page(struct image *img);

struct page* to_page(struct image *img, struct page *pg, int lh);

struct line* to_line(struct image *img, struct line *ligne, int l);

struct paragraph* to_paragraph(struct image *img, struct paragraph *prgph);

struct image* first_char_in_line(struct image *img);

struct image* first_line_in_paragraph(struct image *img);

struct image* first_paragraph_in_page(struct image *img, int lh);

struct image* line_to_image(struct line *ln);

int is_space(struct image *img, int le);

#endif
