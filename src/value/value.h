#ifndef VALUE_H
#define VALUE_H

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

enum Values {
  VAL_INT64
};

struct Value {
  enum Values type;
  union {
    int64_t int64;
  } as;
};

// Convert C type to VM Value
#define TO_INT64(val) (struct Value){ VAL_INT64, { val } }

// Convert VM Value to C type
#define FROM_INT64(val) (val).as.int64

struct ValueList {
  struct Value* values;
  size_t length;
};

void init_valuelist(struct ValueList*);
void resize_valuelist(struct ValueList*, size_t);

#endif
