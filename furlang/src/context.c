#include "./context.h"

#include "./executor.h"

#include <string.h>
#include <assert.h>

Furlang_Context furlang_context_create(Fbc fbc, Furlang_Allocator *allocator) {
  assert(memcmp(fbc.header.magic, (uint8_t[])FBC_MAGIC, sizeof(fbc.header.magic)) == 0);

  Furlang_Context context = furlang_allocator_alloc(allocator, sizeof(*context));
  assert(context);
  memset(context, 0, sizeof(*context));

  context->fbcHeader = fbc.header;
  context->bytecodeLength = fbc.bytecodeLength;
  context->bytecode = fbc.bytecode;

  Fbc_Header_Function entryFunc = fbc.header.functions[fbc.header.entryFunction];
  assert(entryFunc.paramCount == 0);
  _furlang_context_append_executor(context, (Furlang_Position)entryFunc.address);

  return context;
}

void furlang_context_destroy(Furlang_Context context, Furlang_Allocator *allocator) {
  FURLANG_SPARSE_SET_FOREACH(&context->executors, Furlang_Executor, e) {
    _furlang_executor_cleanup(context, _furlang_context_get_executor(context, *e));
  }

  FURLANG_SPARSE_SET_FREE(&context->executors);
  FURLANG_SPARSE_SET_FREE(&context->things);

  FURLANG_DA_FREE(&context->deadExecutors);
  FURLANG_DA_FREE(&context->deadThings);

  furlang_arena_free(&context->thingsDataArena);

  furlang_allocator_free(allocator, context);
}

void furlang_context_collect(Furlang_Context context) {
  FURLANG_SPARSE_SET_FOREACH(&context->things, Furlang_Thing, t) {
    if (_furlang_context_get_thing(context, *t)->referenceCount) continue;
    _furlang_context_remove_thing(context, *t);
  }
}

bool furlang_context_is_running(Furlang_Context context) {
  FURLANG_SPARSE_SET_FOREACH(&context->executors, Furlang_Executor, e) {
    if (furlang_executor_is_running(context, *e)) return true;
  }

  return false;
}

bool furlang_context_step(Furlang_Context context) {
  bool ran = false;
  FURLANG_SPARSE_SET_FOREACH(&context->executors, Furlang_Executor, e) {
    if (furlang_executor_step(context, *e)) ran = true;
  }

  return ran;
}

void furlang_context_run(Furlang_Context context) {
  while (furlang_context_step(context));
}



Furlang_Executor _furlang_context_append_executor(Furlang_Context context, Furlang_Position position) {
  assert(context);

  Furlang_Executor executor = -1;
  if (context->deadExecutors.count) executor = FURLANG_DA_POP_BACK(&context->deadExecutors);
  else executor = FURLANG_SPARSE_SET_SIZE(&context->executors);

  _Furlang_Executor e = { .flags = _FURLANG_EXECUTOR_FLAG_RUNNING };
  FURLANG_DA_APPEND(&e.callStack, ((_Furlang_Call){
    .position = position
  }));

  _furlang_executor_push_scope(&e);

  FURLANG_SPARSE_SET_PUT(&context->executors, Furlang_Executor, executor, e);
  return executor;
}

_Furlang_Executor *_furlang_context_get_executor(Furlang_Context context, Furlang_Executor id) {
  assert(context);

  return &FURLANG_SPARSE_SET_GET(&context->executors, Furlang_Executor, id);
}

void _furlang_context_remove_executor(Furlang_Context context, Furlang_Executor id) {
  assert(context);
  FURLANG_DA_APPEND(&context->deadExecutors, id);
  FURLANG_SPARSE_SET_REMOVE(&context->executors, Furlang_Executor, id);
}



Furlang_Thing _furlang_context_append_thing(Furlang_Context context, Furlang_Thing_Type type) {
  assert(context);

  for (size_t i = 0; i < context->deadThings.count; ++i) {
    _Furlang_Dead_Thing t = context->deadThings.items[i];
    if (t.type != type) continue;
    FURLANG_SPARSE_SET_PUT(&context->things, Furlang_Thing, t.id, ((_Furlang_Thing){ .type = type, .data = t.data }));

    return t.id;
  }

  // New thing
  char *data = furlang_arena_alloc(&context->thingsDataArena, _furlang_thing_type_size_map[type]);
  assert(data);

  Furlang_Thing thing = FURLANG_SPARSE_SET_SIZE(&context->things);
  FURLANG_SPARSE_SET_PUT(&context->things, Furlang_Thing, thing, ((_Furlang_Thing){ .type = type, .data = data }));
  return thing;
}

_Furlang_Thing *_furlang_context_get_thing(Furlang_Context context, Furlang_Thing id) {
  assert(context);

  return &FURLANG_SPARSE_SET_GET(&context->things, Furlang_Thing, id);
}

void _furlang_context_remove_thing(Furlang_Context context, Furlang_Thing id) {
  assert(context);

  _Furlang_Thing *thing = &FURLANG_SPARSE_SET_GET(&context->things, Furlang_Thing, id);
  FURLANG_DA_APPEND(&context->deadThings, ((_Furlang_Dead_Thing){ .type = thing->type, .id = id, .data = thing->data }));
  FURLANG_SPARSE_SET_REMOVE(&context->things, Furlang_Thing, id);
}