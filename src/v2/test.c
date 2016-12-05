#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include "sdl.h"
#include "world.h"

int main(int argc, char* argv[])
{
  if (argc < 4)
  {
    printf("test <network> <charcodes> <image>\n");
    return 0;
  }
  SDL_Surface* img;
  size_t text_length;
  wchar_t* text;
  the_world(argv[1], argv[2], argv[3], &img, &text, &text_length);
  display_image(img);
  display_image(img);
  wprintf(L"%ls", text);
  return 0;
}
