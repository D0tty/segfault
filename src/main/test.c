#define _POSIX_C_SOURCE 199309L
# include <stdio.h>
# include <stddef.h>
# include <wchar.h>
# include <stdlib.h>
# include <locale.h>
# include <math.h>
# include <string.h>
# include <err.h>
# include <time.h>
# include "../util/gaussrand.h"
# include "../util/vector.h"
# include "../util/debug.h"
# include "../util/shuffle.h"
# include "../util/misc.h"
# include "../nn/network.h"
# include "../img/img.h"
# include "../img/decoupage.h"
# include "img_to_txt.h"

void charcodes_load(char charcodes_path[], int** charcodes_ptr,
                    size_t* charcodes_length_ptr)
{
  FILE* fp = fopen(charcodes_path, "r");
  size_t charcodes_length;
  fread(&charcodes_length, sizeof (size_t), 1, fp);
  int* charcodes = malloc(charcodes_length * sizeof (int));
  fread(charcodes, charcodes_length * sizeof (int), 1, fp);
  fclose(fp);
  *charcodes_ptr = charcodes;
  *charcodes_length_ptr = charcodes_length;
}

char* usage(void)
{
  char *txt = "./test <network_file> <charcodes_file> <image>\n";
  return txt;
}

int main(int argc, char *argv[])
{
  //check on args
  if ( argc < 4 )
  {
    printf("%s" ,usage());
    return 2;
  }
  setlocale(LC_ALL, "");
  init_sdl();
  SDL_Surface* sdlimg = load_image(argv[3]);
  tograyscale(sdlimg);
  tobinary(sdlimg);
  struct image *img = image_get_from_SDL(sdlimg);
  SDL_FreeSurface(sdlimg);
  SDL_Quit();

  network* nt = network_load(argv[1]);
  int* charcodes;
  size_t charcodes_length;
  charcodes_load(argv[2], &charcodes, &charcodes_length);
  struct page *pg = get_page(img);

  wchar_t *text = get_buffer(paragraph_compt(pg, 1));
  img_to_buff(nt, charcodes, pg);

  size_t len = paragraph_compt(NULL, 0);
  for (size_t i = 0;  i < len; ++i)
  {
    wprintf(L"%lc", text[i]);
  }

  free(text);
  free_network(nt);
  page_free(pg);

  return 0;
}
