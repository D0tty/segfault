struct image {
	int w, h;
	int *data;
};


#define image_pixel(img, i, j) img->data[img->w * (j) + (i)]

void wait_for_keypressed(void);

static inline
Uint8* pixelref(SDL_Surface *surf, unsigned x, unsigned y);

void putpixel(SDL_Surface *surface, unsigned x, unsigned y, Uint32 pixel);

Uint32 getpixel(SDL_Surface *surface, unsigned x, unsigned y);

void init_sdl(void);

SDL_Surface* load_image(char *path);

SDL_Surface* tograyscale(SDL_Surface* img);

SDL_Surface* tobinary(SDL_Surface* img);

SDL_Surface* display_image(SDL_Surface *img);

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

struct image* image_get_paragraph(struct image *img);
