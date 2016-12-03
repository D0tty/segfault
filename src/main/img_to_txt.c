# include <stdlib.h>
# include <stdio.h>

# include "../img/img.h"
# include "../img/decoupage.h"
# include "../nn/network.h"

/*
 * if len == 0 return a pointer to the buffer, NULL if buffer not init
 * if len != 0 realloc the buffer (no dada ereased and can increase len
 */
wchar_t* get_buffer(size_t len)
{
  static wchar_t *buf = NULL;

  if ( len )
  {
    buf = realloc( buf, len * sizeof(wchar_t) );
  }

  return buf;
}


/*
 * put the wchar_t c in the buffer
 * if the buffer is full after adding c; return 1 else 0
 */
int put_char(wchar_t c)
{
  static size_t index = 0;
  static size_t len = 0;
  len = len ? paragraph_compt(NULL, 0) : len;
  wchar_t *buf = get_buffer(0); //need to init it before put any wchar_t, obvious no ?

  buf[index++] = c; //put the wchar_t increase index

  return index >= len ? 1 : 0; //ternary to ensure 0 or 1 values
}

wchar_t network_recognize(network* nt, int* charcodes, struct image* cc)
{
  double* input = malloc(cc->w * cc->h * sizeof (double));
  size_t output_size = nt->sizes[nt->nb_layers - 1];
  double* activations = malloc(output_size * sizeof (double));
  for (int y = 0; y < cc->h; ++y)
  {
    for (int x = 0; x < cc->w; ++x)
    {
      input[y * cc->w + x] = (double)image_pixel(cc, x, y) - 0.5;
    }
  }
  feedforward(nt, input, activations);
  double max_activation = activations[0];
  size_t max_activation_idx = 0;
  for (size_t i = 1; i < output_size; ++i)
  {
    if (activations[i] > max_activation)
    {
      max_activation = activations[i];
      max_activation_idx = i;
    }
  }
  wchar_t c = charcodes[max_activation_idx];
  free(input);
  free(activations);
  return c;
}

void img_to_buff(network *nt, int* charcodes, struct page *pg)
{
  size_t len = paragraph_compt(pg, 0);
  get_buffer(len);

  while(pg != NULL)
  {
    struct paragraph *prg = pg->current_paragraph;
    while(prg != NULL)
    {
      struct line *ln = prg->current_line;
      while(ln != NULL)
      {
        struct image* cc = ln->current_char;
        wchar_t c = is_img_blank(cc) ? L' ' : network_recognize(nt, charcodes, cc);
        put_char(c);
        ln = ln->next_char;
      }
      put_char(L'\n');
      prg = prg->next_line;
    }
    put_char(L'\t');
    pg = pg->next_paragraph;
  }
}
