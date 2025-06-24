#ifndef   _FURLANG_THING_H_
#define   _FURLANG_THING_H_

#include <stdint.h>

#include "furlang/id.h"

#include "furlang/context.h"

typedef Furlang_Id Furlang_Thing;

typedef int8_t  Furlang_Byte;
typedef int16_t Furlang_Short;
typedef int32_t Furlang_Int;
typedef int64_t Furlang_Long;

typedef enum Furlang_Thing_Type {
  FURLANG_THING_TYPE_INT = 0,

  COUNT_FURLANG_THING_TYPES
} Furlang_Thing_Type;

Furlang_Thing furlang_thing_create(Furlang_Context context, Furlang_Thing_Type type);
Furlang_Thing furlang_thing_clone(Furlang_Context context, Furlang_Thing source);

void furlang_thing_reference(Furlang_Context context, Furlang_Thing thing);
void furlang_thing_unreference(Furlang_Context context, Furlang_Thing thing);

#endif // _FURLANG_THING_H_