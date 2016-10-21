#include <stdlib.h>
#include <stdio.h>
#include <err.h>

void print_list(double* list, size_t length)
{
  printf("[");
  for (size_t i = 0; i < length; ++i)
  {
    printf("%lf", list[i]);
    if (i != length - 1)
    {
      printf(", ");
    }
  }
  printf("]");
}

void print_vector(double* vector, size_t height, size_t width)
{
  printf("[\n");
  for (size_t i = 0; i < height; ++i)
  {
    printf("  ");
    print_list(vector + i * width, width);
    if (i != height - 1)
    {
      printf(",\n");
    }
    else
    {
      printf("\n");
    }
  }
  printf("]");
}
