#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <locale.h>
#include <wchar.h>
#include "sdl.h"
#include "quick_sort.h"
#include "list.h"
#include "../nn/network.h"

#define DIM 28
#define MAX(a, b) (a) > (b) ? (a) : (b)
#define MIN(a, b) (a) < (b) ? (a) : (b)

void mark(int* graph, int* marked, int x, int y, int width, int height,
          int zone, int* x1, int* y1, int* x2, int* y2)
{
  int i = y * width + x;

  if (marked[i] != -1 || !graph[i])
    return;

  *x1 = MIN(x, *x1);
  *y1 = MIN(y, *y1);
  *x2 = MAX(x, *x2);
  *y2 = MAX(y, *y2);

  marked[i] = zone;

  if (y > 0)
    mark(graph, marked, x, y - 1, width, height, zone, x1, y1, x2, y2);
  if (y < height - 1)
    mark(graph, marked, x, y + 1, width, height, zone, x1, y1, x2, y2);

  if (x > 0)
    mark(graph, marked, x - 1, y, width, height, zone, x1, y1, x2, y2);
  if (x < width - 1)
    mark(graph, marked, x + 1, y, width, height, zone, x1, y1, x2, y2);
}

void graph_fill(int* graph, SDL_Surface* img)
{
  // Fill the graph. 1 is a black pixel, 0 is a white pixel.
  for (int y = 0; y < img->h; ++y)
  {
    for (int x = 0; x < img->w; ++x)
    {
      Uint8 R;
      Uint8 G;
      Uint8 B;
      SDL_GetRGB(getpixel(img, x, y), img->format, &R, &G, &B);

      graph[y * img->w + x] = R == 0;
    }
  }
}

struct char_ {
  int x;
  int y;
  int width;
  int height;
  int pixels;
  int* binary;
};

void char_print(struct char_* c, int ratio)
{
  for (int y = 0; y < c->height / ratio; ++y)
  {
    for (int x = 0; x < c->width / ratio; ++x)
    {
      printf(c->binary[(y * ratio) * c->width + (x * ratio)] == 1 ? "#" : " ");
    }
    printf("\n");
  }
  printf("\n");
}

void char_find_all(int* graph, int width, int height, struct char_*** chars_ptr,
                size_t* chars_length_ptr)
{
  int size = width * height;
  int* marked = malloc(size * sizeof (int));
  memset(marked, -1, size * sizeof (int));

  struct char_** chars = malloc(1000 * sizeof (struct char_*));
  size_t allocated = 1000;
  size_t chars_length = 0;
  for (int y = 0; y < height; ++y)
  {
    for (int x = 0; x < width; ++x)
    {
      int i = y * width + x;
      if (!graph[i] || marked[i] != -1)
      {
        continue;
      }

      int min_x = width - 1;
      int min_y = height - 1;
      int max_x = 0;
      int max_y = 0;
      mark(graph, marked, x, y, width, height, (int)chars_length, &min_x,
           &min_y, &max_x, &max_y);

      struct char_* c = malloc(sizeof (struct char_));

      c->x = min_x;
      c->y = min_y;
      c->width = max_x - min_x + 1;
      c->height = max_y - min_y + 1;

      c->pixels = 0;
      c->binary = calloc(1, c->width * c->height * sizeof (int));
      for (int y = 0; y < c->height; ++y)
      {
        for (int x = 0; x < c->width; ++x)
        {
          size_t other_char_idx = (size_t)marked[(c->y + y) * width + c->x + x];
          if (other_char_idx == chars_length)
          {
            ++c->pixels;
            c->binary[y * c->width + x] = 1;
          }
        }
      }

      chars[chars_length] = c;
      ++chars_length;

      if (chars_length == allocated)
      {
        allocated *= 2;
        chars = realloc(chars, allocated * sizeof (struct char_*));
      }
    }
  }

  *chars_ptr = chars;
  *chars_length_ptr = chars_length;

  free(marked);
}

void resize_bilinear(double* out, double* in, int w1, int h1, int w2, int h2)
{
  double x_ratio = (double)w1 / w2;
  double y_ratio = (double)h1 / h2;

  for (int y = 0; y < h2; ++y)
  {
    for (int x = 0; x < w2; ++x)
    {
      double px = x_ratio * ((double)x + .5);
      double py = y_ratio * ((double)y + .5);

      int tx = (int)floor(px - .5);
      int ty = (int)floor(py - .5);
      int bx = tx + 1;
      int by = ty + 1;

      double total_dist = 0.;
      double total = 0.;
      int x1 = tx > 0 ? tx : 0;
      int x2 = bx < w1 ? bx : w1 - 1;
      int y1 = ty > 0 ? ty : 0;
      int y2 = by < h1 ? by : h1 - 1;
      for (int oy = y1; oy <= y2; ++oy)
      {
        for (int ox = x1; ox <= x2; ++ox)
        {
          double val = in[oy * w1 + ox];
          double ax = (double)ox + .5;
          double ay = (double)oy + .5;
          double dist = sqrt(pow(ax - px, 2) + pow(ay - py, 2));
          total_dist += dist;
          total += dist * val;
        }
      }
      if (total_dist == 0.)
      {
        // No idea...
        total = in[y1 * w1 + x2];
      }
      else
      {
        total /= total_dist;
      }

      out[y * w2 + x] = total;
    }
  }
}

struct box {
  int x;
  int y;
  int width;
  int height;
  struct list* children;
};

int box_compare_size_desc(void* v1, void* v2)
{
  struct box* b1 = v1;
  struct box* b2 = v2;
  return b2->width * b2->height - b1->width * b1->height;
}

int box_compare_x_asc(void* v1, void* v2)
{
  struct box* b1 = v1;
  struct box* b2 = v2;
  return b1->x - b2->x;
}

int box_compare_y_asc(void* v1, void* v2)
{
  struct box* b1 = v1;
  struct box* b2 = v2;
  return b1->y - b2->y;
}

struct box* box_create(int x, int y, int width, int height)
{
  struct box* box = malloc(sizeof (struct box));
  box->x = x;
  box->y = y;
  box->width = width;
  box->height = height;
  box->children = list_create();
  return box;
}

void box_dist(struct box* b1, struct box* b2, int* x_dist_ptr, int* y_dist_ptr)
{
  int b1_top = b1->y;
  int b1_bottom = b1_top + b1->height;
  int b1_left = b1->x;
  int b1_right = b1_left + b1->width;
  int b2_top = b2->y;
  int b2_bottom = b2_top + b2->height;
  int b2_left = b2->x;
  int b2_right = b2_left + b2->width;

  int x_dist = b1_left - b2_right;
  if (x_dist < 0)
    x_dist = b2_left - b1_right;
  if (x_dist < 0)
    x_dist = 0;

  int y_dist = b1_top - b2_bottom;
  if (y_dist < 0)
    y_dist = b2_top - b1_bottom;
  if (y_dist < 0)
    y_dist = 0;

  *x_dist_ptr = x_dist;
  *y_dist_ptr = y_dist;
}

struct list* box_group(struct list* boxes,
                       int (*grp)(struct box*, struct box*, void* data),
                       void* data)
{
  struct list* parent_boxes = list_create();
  int* marked = calloc(1, boxes->size * sizeof (int));
  for (size_t i = 0; i < boxes->size; ++i)
  {
    if (marked[i])
      continue;

    struct box* b1 = boxes->elems[i];
    marked[i] = 1;

    struct box* parent_box = malloc(sizeof (struct box));
    parent_box->x = b1->x;
    parent_box->y = b1->y;
    parent_box->width = b1->width;
    parent_box->height = b1->height;
    parent_box->children = list_create();
    list_append(parent_box->children, b1);
    list_append(parent_boxes, parent_box);

    int added = -1;
    do {
      added = 0;

      for (size_t j = 0; j < boxes->size; ++j)
      {
        if (marked[j])
          continue;

        struct box* b2 = boxes->elems[j];
        if (grp(parent_box, b2, data))
        {
          int x1 = MIN(parent_box->x, b2->x);
          int x2 = MAX(parent_box->x + parent_box->width, b2->x + b2->width);
          int y1 = MIN(parent_box->y, b2->y);
          int y2 = MAX(parent_box->y + parent_box->height, b2->y + b2->height);
          parent_box->x = x1;
          parent_box->y = y1;
          parent_box->width = x2 - x1;
          parent_box->height = y2 - y1;
          list_append(parent_box->children, b2);

          marked[j] = 1;
          ++added;
        }
      }
    } while (added != 0);
  }
  free(marked);
  return parent_boxes;
}

int box_contained_x(struct box* b1, struct box* b2)
{
  return b2->x >= b1->x && b2->x + b2->width <= b1->x + b1->width;
}

int box_contained_y(struct box* b1, struct box* b2)
{
  return b2->y >= b1->y && b2->y + b2->height <= b1->y + b1->height;
}

int box_contained(struct box* b1, struct box* b2)
{
  return box_contained_x(b1, b2) && box_contained_y(b1, b2);
}

// GROUP IN LINES
int line_group(struct box* b1, struct box* b2, void* data)
{
  data = NULL;
  int x_dist;
  int y_dist;
  // Biggest element (size reference)
  struct box* box = b1->children->elems[0];
  // Current line y center
  int c1y = box->y + box->height / 2;
  // New box y center
  int c2y = b2->y + b2->height / 2;
  // New line y center
  box_dist(b1, b2, &x_dist, &y_dist);
  return (
    box_contained(b1, b2) ||
    (
      (double)abs(c1y - c2y) < box->height * 0.8 &&
      (double)x_dist < box->height * 3
    )
  );
}

// GROUP IN CHAR
int char_group(struct box* b1, struct box* b2, void* data)
{
  data = NULL;
  // Biggest element (size reference)
  struct box* box = b1->children->elems[0];
  // Current line y center
  int c1x = box->x + box->width / 2;
  // New box y center
  int c2x = b2->x + b2->width / 2;
  // New line y center

  return (
    (double)abs(c1x - c2x) < box->height / 4 ||
    box_contained_x(b1, b2)
  );
}

struct info {
  int min_dist;
  int max_dist;
  double avg_dist;
};

// GROUP IN CHAR
int word_group(struct box* b1, struct box* b2, void* data)
{
  struct info* info = data;
  // Biggest element (size reference)
  int b1right = b1->x + b1->width;
  int b2left = b2->x;
  int b1left = b1->x;
  int b2right = b2->x + b2->width;
  int d1 = b2left - b1right;
  int d2 = b1left - b2right;

  return (
    (d1 >= 0 && (double)d1 < info->avg_dist * 1.5) ||
    (d2 >= 0 && (double)d2 < info->avg_dist * 1.5)
  );
}

int para_group(struct box* b1, struct box* b2, void* data)
{
  data = NULL;
  int b1bottom = b1->y + b1->height;
  int b2top = b2->y;
  int d3 = abs(b2top - b1bottom);

  return (
    (double)d3 < (double)b2->height
  );
}

void resize_keep_ratio(int w, int h, int r, int* ow, int* oh)
{
  *ow = w;
  *oh = h;
  int* b;
  int* s;
  if (w > h)
  {
    b = ow;
    s = oh;
  }
  else
  {
    b = oh;
    s = ow;
  }
  *s = (int)ceil((double)(*s) / (*b) * (double)r);
  *b = r;
}

double ilerp(double a, double b, double w)
{
  return (w - a) / (b - a);
}

void network_activation(network* nt, double* input,
                        double* max_activation_ptr,
                        size_t* max_activation_idx_ptr)
{
  size_t output_size = nt->sizes[nt->nb_layers - 1];
  double* acti = malloc(output_size * sizeof (double));
  feedforward(nt, input, acti);

  double max_activation = acti[0];
  size_t max_activation_idx = 0;

  for (size_t i = 1; i < output_size; ++i)
  {
    if (acti[i] > max_activation)
    {
      max_activation = acti[i];
      max_activation_idx = i;
    }
  }

  *max_activation_ptr = max_activation;
  *max_activation_idx_ptr = max_activation_idx;
  free(acti);
}

void read_chars(struct list* boxes, struct list* text, SDL_Surface* img,
                network* nt)
{
  for (size_t i = 0; i < boxes->size; ++i)
  {
    struct box* box = boxes->elems[i];
    double* char_image = malloc(box->width * box->height * sizeof (double));

    for (int y = 0; y < box->height; ++y)
    {
      for (int x = 0; x < box->width; ++x)
      {
        Uint8 R;
        Uint8 G;
        Uint8 B;
        SDL_GetRGB(getpixel(img, box->x + x, box->y + y), img->format, &R, &G,
                   &B);
        char_image[y * box->width + x] = (double)R / 255;
      }
    }

    int rw;
    int rh;
    resize_keep_ratio(box->width, box->height, DIM, &rw, &rh);

    double* resized_image = malloc(rw * rh * sizeof (double));
    resize_bilinear(resized_image, char_image, box->width, box->height, rw, rh);

    // Readjust black and white values
    double min_value = 1.;
    double max_value = 0.;
    for (int y = 0; y < rh; ++y)
    {
      for (int x = 0; x < rw; ++x)
      {
        min_value = MIN(min_value, resized_image[y * rw + x]);
        max_value = MAX(max_value, resized_image[y * rw + x]);
      }
    }
    for (int y = 0; y < rh; ++y)
    {
      for (int x = 0; x < rw; ++x)
      {
        resized_image[y * rw + x] = ilerp(min_value, max_value,
                                          resized_image[y * rw + x]);
      }
    }

    double* final_image = malloc(DIM * DIM * sizeof (double));
    for (int y = 0; y < DIM; ++y)
    {
      for (int x = 0; x < DIM; ++x)
      {
        final_image[y * DIM + x] = -0.5;
      }
    }

    int x_offset = (DIM - rw) / 2;
    int y_offset = (DIM - rh) / 2;
    for (int y = 0; y < rh; ++y)
    {
      for (int x = 0; x < rw; ++x)
      {
        size_t final_idx = (y_offset + y) * DIM + x_offset + x;
        final_image[final_idx] = .5 - resized_image[y * rw + x];
      }
    }

    // SDL_Surface* visu = SDL_CreateRGBSurface(0, DIM, DIM, 32, 0, 0, 0, 0);
    // for (int y = 0; y < DIM; ++y)
    // {
    //   for (int x = 0; x < DIM; ++x)
    //   {
    //     int grey = round((.5 - final_image[y * DIM + x]) * 255);
    //     Uint32 pix = SDL_MapRGB(img->format, grey, grey, grey);
    //     putpixel(visu, x, y, pix);
    //   }
    // }
    //
    // display_image(visu);

    double max_acti;
    size_t max_acti_idx;
    network_activation(nt, final_image, &max_acti, &max_acti_idx);
    int* char_ptr = malloc(sizeof (int));
    *char_ptr = max_acti_idx;

    list_append(text, char_ptr);
  }
}

void read_words(struct list* boxes, struct list* text, SDL_Surface* img,
                network* nt)
{
  for (size_t i = 0; i < boxes->size; ++i)
  {
    struct box* box = boxes->elems[i];
    quick_sort(box->children->elems, box->children->size, box_compare_x_asc);
    read_chars(box->children, text, img, nt);
    int* char_ptr = malloc(sizeof (int));
    *char_ptr = -2;
    list_append(text, char_ptr);
  }
}

void read_lines(struct list* boxes, struct list* text, SDL_Surface* img,
                network* nt)
{
  for (size_t i = 0; i < boxes->size; ++i)
  {
    struct box* box = boxes->elems[i];
    quick_sort(box->children->elems, box->children->size, box_compare_x_asc);
    read_words(box->children, text, img, nt);
    int* char_ptr = malloc(sizeof (int));
    *char_ptr = -3;
    list_append(text, char_ptr);
  }
}

void read_paragraphs(struct list* boxes, struct list* text, SDL_Surface* img,
                     network* nt)
{
  for (size_t i = 0; i < boxes->size; ++i)
  {
    int* begin_ptr = malloc(sizeof (int));
    *begin_ptr = -5;
    list_append(text, begin_ptr);
    struct box* box = boxes->elems[i];
    quick_sort(box->children->elems, box->children->size, box_compare_y_asc);
    read_lines(box->children, text, img, nt);
    int* end_ptr = malloc(sizeof (int));
    *end_ptr = -4;
    list_append(text, end_ptr);
  }
}

int* charcodes_load(char charcodes_path[])
{
  FILE* fp = fopen(charcodes_path, "r");
  size_t charcodes_length;
  fread(&charcodes_length, sizeof (size_t), 1, fp);
  int* charcodes = malloc(charcodes_length * sizeof (int));
  fread(charcodes, charcodes_length * sizeof (int), 1, fp);
  fclose(fp);
  return charcodes;
}

void display_boxes(SDL_Surface* img, struct list* boxes, Uint32* colors,
                   size_t depth)
{
  Uint32 color = colors[0];
  for (size_t i = 0; i < boxes->size; ++i)
  {
    struct box* box = boxes->elems[i];
    for (int x = 0; x < box->width; ++x)
    {
      putpixel(img, box->x + x, box->y, color);
      putpixel(img, box->x + x, box->y + box->height - 1, color);
    }
    for (int y = 0; y < box->height; ++y)
    {
      putpixel(img, box->x, box->y + y, color);
      putpixel(img, box->x + box->width - 1, box->y + y, color);
    }
    if (depth != 0)
      display_boxes(img, box->children, colors + 1, depth - 1);
  }
}

int text_print(wchar_t chars[], struct list* text, int* charcodes)
{
  int size = 0;
  for (size_t i = 0; i < text->size; ++i)
  {
    int* c = text->elems[i];
    int v = *c;
    if (v == -2)
      size += swprintf(chars + size, 100, L" ");
    else if (v == -3)
      size += swprintf(chars + size, 100, L"\n");
    else if (v == -4)
      size += swprintf(chars + size, 100, L"\n\n");
    else if (v == -5)
      size += swprintf(chars + size, 100, L"");
    else
    {
      wchar_t wc = charcodes[v];
      size += swprintf(chars + size, 100, L"%lc", wc);
    }
  }
  return size;
}

void the_world(char network_path[], char charcodes_path[], char image_path[],
               SDL_Surface** visu_ptr, wchar_t** text_ptr,
               size_t* text_size_ptr)
{
  setlocale(LC_ALL, "");

  network* nt = network_load(network_path);
  int* charcodes = charcodes_load(charcodes_path);

  // TODO: Free surfaces
  SDL_Surface* img = load_image(image_path);
  SDL_Surface* grayscale = SDL_ConvertSurface(img, img->format, SDL_SWSURFACE);
  tograyscale(grayscale);
  SDL_Surface* binary = SDL_ConvertSurface(grayscale, grayscale->format,
                                           SDL_SWSURFACE);
  tobinary(binary, 180);

  int* graph = malloc(img->h * img->w * sizeof (int*));
  graph_fill(graph, binary);

  struct char_** chars;
  size_t chars_length;
  char_find_all(graph, img->w, img->h, &chars, &chars_length);

  struct list* elem_boxes = list_create();
  for (size_t i = 0; i < chars_length; ++i)
  {
    struct char_* c = chars[i];
    struct box* elem_box = box_create(c->x, c->y, c->width, c->height);
    list_append(elem_box->children, c);
    list_append(elem_boxes, elem_box);
  }

  // So that the first element considered always takes the most area already.
  // Mose useful for char elem and char grouping.
  quick_sort(elem_boxes->elems, elem_boxes->size, box_compare_size_desc);

  struct list* line_boxes = box_group(elem_boxes, line_group, NULL);
  for (size_t i = 0; i < line_boxes->size; ++i)
  {
    struct box* line_box = line_boxes->elems[i];
    struct list* elem_boxes = line_box->children;
    quick_sort(elem_boxes->elems, elem_boxes->size, box_compare_x_asc);
    struct list* char_boxes = box_group(elem_boxes, char_group, NULL);
    quick_sort(char_boxes->elems, char_boxes->size, box_compare_x_asc);
    int min_dist = 10000;
    int max_dist = -10000;
    double avg_dist = 0.;
    for (size_t j = 0; j < char_boxes->size - 1; ++j)
    {
      struct box* char_box = char_boxes->elems[j];
      struct box* next_char_box = char_boxes->elems[j + 1];
      int dist = next_char_box->x - (char_box->x + char_box->width);
      min_dist = min_dist >= 0 ? MIN(min_dist, dist) : min_dist;
      max_dist = MAX(max_dist, dist);
      avg_dist += (double)dist;
    }
    struct info* info = malloc(sizeof (struct info));
    info->min_dist = min_dist;
    info->max_dist = max_dist;
    info->avg_dist = (avg_dist / (double)(char_boxes->size - 1));
    struct list* word_boxes = box_group(char_boxes, word_group, info);
    line_box->children = word_boxes;
    // TODO: Free children
  }
  quick_sort(line_boxes->elems, line_boxes->size, box_compare_y_asc);
  struct list* para_boxes = box_group(line_boxes, para_group, NULL);

  quick_sort(para_boxes->elems, para_boxes->size, box_compare_y_asc);

  struct list* text = list_create();
  read_paragraphs(para_boxes, text, grayscale, nt);

  SDL_Surface* visu = SDL_ConvertSurface(img, img->format, SDL_SWSURFACE);
  Uint32* colors = malloc(5 * sizeof (Uint32));
  colors[0] = SDL_MapRGB(img->format, 255, 0, 0);
  colors[1] = SDL_MapRGB(img->format, 0, 255, 0);
  colors[2] = SDL_MapRGB(img->format, 0, 0, 255);
  colors[3] = SDL_MapRGB(img->format, 255, 140, 50);
  colors[4] = SDL_MapRGB(img->format, 52, 255, 230);
  display_boxes(visu, para_boxes, colors, 5);
  *visu_ptr = visu;

  wchar_t* str = calloc(1, text->size * 4 * sizeof (wchar_t));
  *text_ptr = str;
  *text_size_ptr = text_print(str, text, charcodes);


  // TODO: free all the boxes
  free_network(nt);
  free(charcodes);
  free(graph);
  SDL_FreeSurface(img);
}
