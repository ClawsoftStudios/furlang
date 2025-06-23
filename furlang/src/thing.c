#include "./thing.h"

#include "./context.h"

Furlang_Thing furlang_thing_create(Furlang_Context context, Furlang_Thing_Type type) {
  assert(context);
  assert(type < COUNT_FURLANG_THING_TYPES);
  return _furlang_context_append_thing(context, type);
}



size_t _furlang_thing_type_size_map[COUNT_FURLANG_THING_TYPES] = {
  [FURLANG_THING_TYPE_INT] = sizeof(Furlang_Int),
};