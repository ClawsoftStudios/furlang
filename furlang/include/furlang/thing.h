#ifndef   _FURLANG_THING_H_
#define   _FURLANG_THING_H_

#include <stdint.h>

#include "furlang/id.h"

#include "furlang/context.h"

typedef Furlang_Id Furlang_Thing;

typedef int32_t Furlang_Int;

typedef enum Furlang_Thing_Type {
  FURLANG_THING_TYPE_INT = 0,

  COUNT_FURLANG_THING_TYPES
} Furlang_Thing_Type;

Furlang_Thing furlang_thing_create(Furlang_Context context, Furlang_Thing_Type type);

#endif // _FURLANG_THING_H_