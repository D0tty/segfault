#define _POSIX_C_SOURCE 199309L
# include <stdio.h>
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

char* usage(void)
{
  char *txt = "./test <network_file> <image>\n";
  return txt;
}

int main(int argc, char *argv[])
{
  //check on args
  if ( argc < 3 )
  {
    printf("%s" ,usage());
    return 2;
  }
  setlocale(LC_ALL, "");
  init_sdl();
  SDL_Surface* sdlimg = load_image(argv[2]);
  tograyscale(sdlimg);
  tobinary(sdlimg);
  struct image *img = image_get_from_SDL(sdlimg);
  SDL_FreeSurface(sdlimg);
  SDL_Quit();


  network* nt = network_load(argv[1]);
  struct page *pg = get_page(img);

  wchar_t* text = get_buffer(paragraph_compt(pg, 1));
  img_to_buff(nt, pg);

  printf("%ls\n", text);


  free(text);
  free_network(nt);
  page_free(pg);

  return 0;
}
