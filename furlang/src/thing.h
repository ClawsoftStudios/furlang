#ifndef   __FURLANG_THING_H_
#define   __FURLANG_THING_H_

#include "furlang/thing.h"

#define _FURLANG_DEAD_THING ((Furlang_Thing)-1)

typedef struct _Furlang_Thing {
  Furlang_Thing_Type type;
  size_t referenceCount;
  char *data;
} _Furlang_Thing;

typedef enum _Furlang_Thing_Op {
  _FURLANG_THING_OP_ADD = 0,
  _FURLANG_THING_OP_SUB,
  _FURLANG_THING_OP_MUL,
  _FURLANG_THING_OP_DIV,
  _FURLANG_THING_OP_MOD,

  _COUNT_FURLANG_THING_OPS
} _Furlang_Thing_Op;

typedef Furlang_Thing (*_Furlang_Thing_Op_Func)(Furlang_Context, _Furlang_Thing *, _Furlang_Thing *);
typedef _Furlang_Thing_Op_Func _Furlang_Thing_Op_Funcs[_COUNT_FURLANG_THING_OPS];

extern const size_t _furlang_thing_type_size_map[COUNT_FURLANG_THING_TYPES];

#define _FURLANG_THING_OP(a, b) a+(b*COUNT_FURLANG_THING_TYPES)
extern const _Furlang_Thing_Op_Funcs _furlang_thing_ops[COUNT_FURLANG_THING_TYPES*COUNT_FURLANG_THING_TYPES]; // Thing operations map

#endif // __FURLANG_THING_H_