#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <err.h>
#include <time.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include "../nn/network.h"
#include "../util/qsort.h"

#define MAX_CHAR_CODE 1000
#define INPUT_DIM 28
#define INPUT_SIZE (size_t)(INPUT_DIM * INPUT_DIM)
#define DATA "data"
#define MAX_TRAINING_DATA 200000

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

void load_training_data(char data_path[], training_datum*** training_data_ptr,
                        size_t* training_data_length_ptr,
                        size_t* char_codes_length_ptr)
{
  DIR *dir;
  struct dirent *ent;
  int* char_codes = malloc(MAX_CHAR_CODE * sizeof (int));
  size_t char_codes_length = 0;

  if ((dir = opendir(data_path)) != NULL)
  {
    while ((ent = readdir(dir)) != NULL)
    {
      int char_code;
      int read = sscanf(ent->d_name, "%i", &char_code);
      if (read == 1)
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
    errx(1, "Could not open directory %s", data_path);
  }

  quick_sort(char_codes, char_codes + char_codes_length);

  training_datum** training_data = malloc(MAX_TRAINING_DATA *
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

    sprintf(char_dir, "%s/%i", data_path, char_code);
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
        if (training_data_length > MAX_TRAINING_DATA)
        {
          errx(1, "Too many training data");
        }
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
                        size_t training_data_length)
{
  double* prev_output = NULL;
  for (size_t i = 0; i < training_data_length; ++i)
  {
    if (training_data[i]->output != prev_output)
    {
      prev_output = training_data[i]->output;
      // All training data of the same char share the same output array.
      free(training_data[i]->output);
    }
    free(training_data[i]->input);
    free(training_data[i]);
  }
  free(training_data);
}

void test_train(char data_path[], char output_path[],
                unsigned long long epochs, size_t hidden_layer)
{
  training_datum** training_data;
  size_t training_data_length;
  size_t char_codes_length;

  load_training_data(data_path, &training_data, &training_data_length,
                     &char_codes_length);

  size_t sizes[] = { INPUT_SIZE, hidden_layer, char_codes_length };
  warnx("Creating network with layers %zu -> %zu -> %zu", INPUT_SIZE,
        hidden_layer, char_codes_length);
  network* nt = create_network(sizes, 3);
  small_weights_init(nt);

  sgd(nt, training_data, training_data_length, epochs, 10, .05, 0.,
      output_path);

  warnx("Freeing network");
  free_network(nt);

  warnx("Freeing training data");
  free_training_data(training_data, training_data_length);
}

void test_continue_train(char data_path[], char output_path[],
                         unsigned long long epochs, char filename[])
{
  training_datum** training_data;
  size_t training_data_length;
  size_t char_codes_length;

  load_training_data(data_path, &training_data, &training_data_length,
                     &char_codes_length);

  network* nt = network_load(filename);

  sgd(nt, training_data, training_data_length, epochs, 10, .05, 0.,
      output_path);

  warnx("Freeing network");
  free_network(nt);

  warnx("Freeing training data");
  free_training_data(training_data, training_data_length);
}

void print_usage()
{
  printf("Usage:\n");
  printf("  train new <data_path> <output_path> <epochs> <hidden_layer_neurons>\n");
  printf("  train continue <data_path> <output_path> <epochs> <path_to_network>\n\n");
  printf("Note: make sure <output_path> is an existing directory.\n\n");
  printf("Examples:\n");
  printf("$ train new data networks 30 240\n");
  printf("# Train a new network with 240 neurons in the hidden layer for 30 epochs.\n");
  printf("# Load training data from ./data.\n");
  printf("# Save each epoch as epochN.network in the ./networks directory.\n\n");
  printf("$ train continue data networks2 60 networks/epoch29.network\n");
  printf("# Continue training the network saved at networks/epoch29.network for 60 additional epochs.\n");
  printf("# Load training data from ./data.\n");
  printf("# Save each epoch as epochN.network in the ./networks2 directory.\n");

}

int main(int argc, char* argv[])
{
  srand(time(NULL));
  if (argc < 6)
  {
    print_usage();
  }
  else
  {
    char* cmd = argv[1];
    char* data_path = argv[2];
    char* output_path = argv[3];
    unsigned long long epochs;
    sscanf(argv[4], "%llu", &epochs);
    if (strcmp(cmd, "new") == 0)
    {
      size_t hidden_layer;
      sscanf(argv[5], "%zu", &hidden_layer);
      test_train(data_path, output_path, epochs, hidden_layer);
    }
    else if (strcmp(cmd, "continue") == 0)
    {
      test_continue_train(data_path, output_path, epochs, argv[5]);
    }
    else
    {
      print_usage();
    }
  }
  return 0;
}