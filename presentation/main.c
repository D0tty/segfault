#include "img.h"
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <err.h>
#include <stdlib.h>
#include <stdio.h>
#include "pixel_operations.h"


int main(int choix, char **argv)
{
  init_sdl();
  SDL_Surface* image = load_image(argv[1]);
  display_image(image);
  if (choix == 1)
  {
    image = tograyscale(image);
  }
  if (choix == 2)
  {
    image = tobinary(image);
  }

  display_image(image);
}
