#ifndef IMG_H
#define IMG_H

# include <SDL/SDL.h>

struct image {
  int w, h;
  int *data;
};


#define image_pixel(img, i, j) img->data[img->w * (j) + (i)]


SDL_Surface* to_sdl_image(struct image *img);

struct image* image_create(int w, int h);

void image_fill(struct image *img);

void image_print(struct image *img);

void image_prety_print(struct image *img);

struct image* image_get_from_SDL(SDL_Surface* sdlimg);

struct image* image_get_rect(struct image *img, int x, int y, int z, int t);

void image_free(struct image *img);

int is_line_blank(struct image *img, int y);

int is_column_blank(struct image *img, int x);

int is_img_blank(struct image *img);

struct image* image_get_paragraph(struct image *img);

struct image* image_merge(struct image *img1, struct image *img2);

struct image* lateral_cut(struct image *img);

struct image* image_merge_vertical(struct image *img1, struct image *img2);

struct image* resizing(struct image *img, int nt);

SDL_Surface* noise_reduction(SDL_Surface *img);

SDL_Surface* right_rotation(SDL_Surface *img);

SDL_Surface* left_rotation(SDL_Surface *img);

SDL_Surface* rotation(SDL_Surface *img, double angle);
#endif
