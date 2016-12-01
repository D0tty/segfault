#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <err.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include "../nn/network.h"
#include "../util/qsort.h"

#define MAX_CHAR_CODE 1000
#define INPUT_DIM 28
#define INPUT_SIZE INPUT_DIM * INPUT_DIM
#define DATA "data2"
#define FONTS 112
#define ROT 3
#define CHAR_DATA_SIZE FONTS * ROT

static inline
Uint8* pixelref(SDL_Surface *surf, unsigned x, unsigned y)
{
  int bpp = surf->format->BytesPerPixel;
  return (Uint8*)surf->pixels + y * surf->pitch + x * bpp;
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

void image_to_input(double* input, SDL_Surface* img)
{
  for (int y = 0; y < img->h; ++y)
  {
    for (int x = 0; x < img->w; ++x)
    {
      Uint8 r, g, b;
      SDL_GetRGB(getpixel(img, x, y), img->format, &r, &g, &b);
      input[y * img->w + x] = 1. - (double)r / 255.;
    }
  }
}

void debug_training_datum(training_datum* d, size_t output_size)
{
  printf("input:\n");
  for (int y = 0; y < INPUT_DIM; ++y)
  {
    for (int x = 0; x < INPUT_DIM; ++x)
    {
      if (d->input[y * INPUT_DIM + x] > .5)
      {
        printf("#");
      }
      else
      {
        printf(" ");
      }
    }
    printf("\n");
  }
  printf("output:\n");
  for (size_t i = 0; i < output_size; ++i)
  {
    printf("%.0f ", d->output[i]);
  }
  printf("\n");
}

void debug_activations(double* activations, size_t activations_size)
{
  printf("activations:\n");
  for (size_t i = 0; i < activations_size; ++i)
  {
    printf("%.2f ", activations[i]);
  }
  printf("\n");
}

void load_training_data(training_datum*** training_data_ptr,
                        size_t* training_data_length_ptr,
                        size_t* char_codes_length_ptr)
{
  DIR *dir;
  struct dirent *ent;
  int* char_codes = malloc(MAX_CHAR_CODE * sizeof (int));
  size_t char_codes_length = 0;

  char data_dir[] = "../../" DATA;
  if ((dir = opendir(data_dir)) != NULL)
  {
    while ((ent = readdir(dir)) != NULL)
    {
      int char_code;
      sscanf(ent->d_name, "%i", &char_code);
      if (char_code != 0)
      {
        char_codes[char_codes_length] = char_code;
        ++char_codes_length;
        if (char_codes_length == MAX_CHAR_CODE)
        {
          errx(1, "Too many char codes");
        }
      }
    }
    closedir(dir);
  } else {
    errx(1, "Could not open directory %s", data_dir);
  }

  quick_sort(char_codes, char_codes + char_codes_length);

  // TODO: Remove
  // char_codes_length = 100;

  training_datum** training_data = malloc(char_codes_length * CHAR_DATA_SIZE *
                                          sizeof (training_datum*));
  size_t training_data_length = 0;

  char char_dir[255];
  char char_image[255];
  for (size_t i = 0; i < char_codes_length; ++i)
  {
    int char_code = char_codes[i];
    warnx("Loading training set %zu (%i)", i, char_code);

    double* output = malloc(char_codes_length * sizeof (double));
    for (size_t j = 0; j < char_codes_length; ++j)
    {
      output[j] = 0.;
    }
    output[i] = 1.;

    sprintf(char_dir, "%s/%i", data_dir, char_code);
    int j = 0;
    if ((dir = opendir(char_dir)) != NULL)
    {
      while ((ent = readdir(dir)) != NULL)
      {
        sprintf(char_image, "%s/%s", char_dir, ent->d_name);
        SDL_Surface* img = IMG_Load(char_image);
        if (!img) {
          // warnx("Could not open image %s", char_image);
          continue;
        }
        double* input = malloc(INPUT_SIZE * sizeof (double));
        image_to_input(input, img);
        training_data[training_data_length] = malloc(sizeof (training_datum));
        training_data[training_data_length]->input = input;
        training_data[training_data_length]->output = output;
        ++training_data_length;
        SDL_FreeSurface(img);
        ++j;
      }
      closedir(dir);
    } else {
      errx(1, "Could not open directory %s", char_dir);
    }
  }

  *training_data_ptr = training_data;
  *training_data_length_ptr = training_data_length;
  *char_codes_length_ptr = char_codes_length;
}

void free_training_data(training_datum** training_data,
                        size_t training_data_length,
                        size_t char_codes_length)
{
  for (size_t i = 0; i < training_data_length; ++i)
  {
    if (i % (char_codes_length * CHAR_DATA_SIZE) == 0)
    {
      // All training data of the same char share the same output array.
      free(training_data[i]->output);
    }
    free(training_data[i]->input);
    free(training_data[i]);
  }
  free(training_data);
}

void test_train()
{
  training_datum** training_data;
  size_t training_data_length;
  size_t char_codes_length;

  load_training_data(&training_data, &training_data_length, &char_codes_length);

  size_t sizes[] = { INPUT_SIZE, 30, char_codes_length };
  network* nt = create_network(sizes, 3);
  small_weights_init(nt);

  sgd(nt, training_data, training_data_length, 30, 10, .05, 0.);

  warnx("Freeing network");
  free_network(nt);

  warnx("Freeing training data");
  free_training_data(training_data, training_data_length, char_codes_length);
}

void test_continue_train()
{
  training_datum** training_data;
  size_t training_data_length;
  size_t char_codes_length;

  load_training_data(&training_data, &training_data_length, &char_codes_length);

  network* nt = network_load("networks/epoch29.network");

  sgd(nt, training_data, training_data_length, 30, 10, .05, 0.);
//
  warnx("Freeing network");
  free_network(nt);

  warnx("Freeing training data");
  free_training_data(training_data, training_data_length, char_codes_length);
}

void test_verify()
{
  training_datum** training_data;
  size_t training_data_length;
  size_t char_codes_length;

  load_training_data(&training_data, &training_data_length, &char_codes_length);

  network* nt = network_load("nt.network");

  double* activations = malloc(char_codes_length * sizeof (double));

  feedforward(nt, training_data[10000]->input, activations);
  debug_training_datum(training_data[10000], char_codes_length);
  debug_activations(activations, char_codes_length);

  warnx("Freeing network");
  free_network(nt);

  warnx("Freeing training data");
  free_training_data(training_data, training_data_length, char_codes_length);
}

int main()
{
  // test_train();
  // test_verify();
  test_continue_train();
  return 0;
}