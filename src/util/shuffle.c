#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Fisher-Yates shuffle
void* shuffle(void* source, size_t length, size_t size)
{
  void* output = malloc(length * size);
  char* source_char = source;
  char* output_char = output;
  for (size_t i = 0; i < length; ++i)
  {
    size_t j = rand() % (i + 1); // Random number in the range [0, i + 1]
    if (j != i)
    {
      memcpy(output_char + (i * size), output_char + (j * size), size);
    }
    memcpy(output_char + (j * size), source_char + (i * size), size);
  }
  return output;
}
