#include <stdlib.h>
#include "qsort.h"

static inline
void swap(int *a, int *b)
{
  int c = *a;
  *a = *b;
  *b = c;
}

int* partition(int *begin, int *end)
{
  size_t length = end - begin;
  int *pivot = begin + length / 2;
  int pval = *pivot;
  swap(pivot, (end - 1));
  pivot = begin;
  for (int *cursor = begin; cursor != end; ++cursor)
  {
    if (*cursor < pval)
    {
      swap(pivot, cursor);
      ++pivot;
    }
  }
  swap(pivot, (end - 1));
  return pivot;
}

void quick_sort(int *begin, int *end)
{
  if (end - begin > 1)
  {
    int *pivot = partition(begin, end);
    quick_sort(begin, pivot);
    quick_sort(pivot + 1, end);
  }
}
