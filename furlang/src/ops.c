#include "./thing.h"
#include "./context.h"

#include <assert.h>

// static Furlang_Thing ta_tb_op(Furlang_Context context, _Furlang_Thing *a, _Furlang_Thing *b);

#define normal(t1, t2, tr, type, op, name)                                                   \
  static Furlang_Thing name(Furlang_Context context, _Furlang_Thing *a, _Furlang_Thing *b) { \
    Furlang_Thing thing = _furlang_context_append_thing(context, type);                      \
    _Furlang_Thing *t = _furlang_context_get_thing(context, thing);                          \
    *(tr*)t->data = (*(t1*)a->data) op (*(t2*)b->data);                                      \
                                                                                             \
    return thing;                                                                            \
  }

normal(Furlang_Int, Furlang_Int, Furlang_Int, FURLANG_THING_TYPE_INT, +, int_int_add)
normal(Furlang_Int, Furlang_Int, Furlang_Int, FURLANG_THING_TYPE_INT, -, int_int_sub)
normal(Furlang_Int, Furlang_Int, Furlang_Int, FURLANG_THING_TYPE_INT, *, int_int_mul)
normal(Furlang_Int, Furlang_Int, Furlang_Int, FURLANG_THING_TYPE_INT, /, int_int_div)
normal(Furlang_Int, Furlang_Int, Furlang_Int, FURLANG_THING_TYPE_INT, %, int_int_mod)

const _Furlang_Thing_Op_Funcs _furlang_thing_ops[COUNT_FURLANG_THING_TYPES*COUNT_FURLANG_THING_TYPES] = {
  [_FURLANG_THING_OP(FURLANG_THING_TYPE_INT, FURLANG_THING_TYPE_INT)] = {
    [_FURLANG_THING_OP_ADD] = int_int_add,
    [_FURLANG_THING_OP_SUB] = int_int_sub,
    [_FURLANG_THING_OP_MUL] = int_int_mul,
    [_FURLANG_THING_OP_DIV] = int_int_div,
    [_FURLANG_THING_OP_MOD] = int_int_mod,
  },
};