#include "img.h"
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <err.h>
#include <stdlib.h>
#include <stdio.h>
#include "pixel_operations.h"


int main(int argc, char **argv)
{
  if(argc < 3)
  {
    return 1;
  }
  init_sdl();
  SDL_Surface* image = load_image(argv[2]);
  unsigned long choix = strtoul(argv[1],NULL,10);
  if (choix == 1)
  {
    warnx("1\n");
    image = tograyscale(image);
  }
  if (choix == 2)
  {
    image = tobinary(image);
  }

  display_image(image);

  return 0;
}
