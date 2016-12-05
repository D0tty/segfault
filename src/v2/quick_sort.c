#include <stdlib.h>

static inline
void swap(void** a, void** b)
{
  void* c = *a;
  *a = *b;
  *b = c;
}

static inline
size_t partition(void** list, size_t length, int (*compare)(void*, void*))
{
  size_t pivot = length / 2;
  void* pval = list[pivot];
  swap(list + pivot, list + length - 1);
  pivot = 0;
  for (size_t i = 0; i < length - 1; ++i)
  {
    if (compare(list[i], pval) <= 0)
    {
      swap(list + pivot, list + i);
      ++pivot;
    }
  }
  swap(list + pivot, list + length - 1);
  return pivot;
}

void quick_sort(void** list, size_t length, int (*compare)(void*, void*))
{
  if (length > 1)
  {
    size_t pivot = partition(list, length, compare);
    void** other_part = (void**)((char**)list + pivot);
    quick_sort(list, pivot, compare);
    quick_sort(other_part, length - pivot, compare);
  }
}
