#ifndef   __FURLANG_THING_H_
#define   __FURLANG_THING_H_

#include "furlang/thing.h"

#define _FURLANG_DEAD_THING ((Furlang_Thing)-1)

typedef struct _Furlang_Thing {
  Furlang_Thing_Type type;
  size_t referenceCount;
  char *data;
} _Furlang_Thing;

extern size_t _furlang_thing_type_size_map[COUNT_FURLANG_THING_TYPES];

#endif // __FURLANG_THING_H_