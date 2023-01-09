// value.c

#include "value.h"
#include <stdio.h>

void init_valuelist(struct ValueList* list) {
  list->length = 0;
  list->values = calloc(list->length, sizeof(struct Value));
}

void resize_valuelist(struct ValueList* list, size_t new_size) {
  list->length = new_size;
  struct Value* ptr =
    reallocarray(list->values, list->length, sizeof(struct Value));
  if(ptr)
    list->values = ptr;
  else perror("failed to reallocate valuelist");
}
