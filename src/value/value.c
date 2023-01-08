// value.c

#include "value.h"

void init_valuelist(struct ValueList* list) {
  list->length = 4;
  list->values = calloc(list->length, sizeof(struct Value));
}

void resize_valuelist(struct ValueList* list, size_t new_size) {
  list->length = new_size;
  list->values = reallocarray(list->values, list->length, sizeof(struct Value));
}
