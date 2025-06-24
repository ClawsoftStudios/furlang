#include "./thing.h"

#include "./context.h"

#include <string.h>

Furlang_Thing furlang_thing_create(Furlang_Context context, Furlang_Thing_Type type) {
  assert(context);
  assert(type < COUNT_FURLANG_THING_TYPES);
  return _furlang_context_append_thing(context, type);
}

Furlang_Thing furlang_thing_clone(Furlang_Context context, Furlang_Thing source) {
  assert(context);

  _Furlang_Thing *src = _furlang_context_get_thing(context, source);
  Furlang_Thing thing = _furlang_context_append_thing(context, src->type);
  _Furlang_Thing *t = _furlang_context_get_thing(context, thing);

  memcpy(t->data, src->data, _furlang_thing_type_size_map[t->type]);

  return thing;
}

void furlang_thing_reference(Furlang_Context context, Furlang_Thing thing) {
  assert(context);
  _Furlang_Thing *t = _furlang_context_get_thing(context, thing);
  ++t->referenceCount;
}

void furlang_thing_unreference(Furlang_Context context, Furlang_Thing thing) {
  assert(context);
  _Furlang_Thing *t = _furlang_context_get_thing(context, thing);
  assert(t->referenceCount);

  --t->referenceCount;
}



size_t _furlang_thing_type_size_map[COUNT_FURLANG_THING_TYPES] = {
  [FURLANG_THING_TYPE_INT] = sizeof(Furlang_Int),
};