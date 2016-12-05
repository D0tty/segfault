#include <stdlib.h>
#include "list.h"

struct list* list_create()
{
  struct list* list = malloc(sizeof (struct list));
  list->size = 0;
  list->capacity = 10;
  list->elems = malloc(list->capacity * sizeof (void*));
  return list;
}

void list_append(struct list* list, void* elem)
{
  if (list->size == list->capacity)
  {
    list->capacity *= 2;
    list->elems = realloc(list->elems, list->capacity * sizeof (void*));
  }
  list->elems[list->size] = elem;
  ++list->size;
}
