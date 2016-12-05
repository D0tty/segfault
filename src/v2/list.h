#ifndef LIST_H
#define LIST_H

struct list {
  size_t capacity;
  size_t size;
  void** elems;
};

struct list* list_create();

void list_append(struct list* list, void* elem);

#endif
