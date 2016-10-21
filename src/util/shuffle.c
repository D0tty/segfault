#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Fisher-Yates shuffle
void shuffle(void* dst, void* src, size_t length, size_t size)
{
  char* src_char = src;
  char* dst_char = dst;
  for (size_t i = 0; i < length; ++i)
  {
    size_t j = rand() % (i + 1); // Random number in the range [0, i + 1]
    if (j != i)
    {
      memcpy(dst_char + (i * size), dst_char + (j * size), size);
    }
    memcpy(dst_char + (j * size), src_char + (i * size), size);
  }
}
