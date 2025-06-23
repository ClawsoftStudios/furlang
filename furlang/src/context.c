#include "./context.h"

#include "./executor.h"

#include <string.h>
#include <assert.h>

Furlang_Context furlang_context_create(Furlang_Allocator *allocator) {
  Furlang_Context context = furlang_allocator_alloc(allocator, sizeof(*context));
  assert(context);
  memset(context, 0, sizeof(*context));

  return context;
}

void furlang_context_destroy(Furlang_Context context, Furlang_Allocator *allocator) {
  FURLANG_SPARSE_SET_FOREACH(&context->executors, Furlang_Executor, e) {
    furlang_executor_destroy(context, *e);
  }

  furlang_allocator_free(allocator, context);
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
    if (furlang_executor_is_running(context, *e)) ran = true;
  }

  return ran;
}

void furlang_context_run(Furlang_Context context) {
  while (furlang_context_step(context));
}



Furlang_Executor _furlang_context_append_executor(Furlang_Context context) {
  assert(context);

  Furlang_Executor executor = -1;
  if (context->deadExecutors.count) executor = FURLANG_DA_POP_BACK(&context->deadExecutors);
  else executor = FURLANG_SPARSE_SET_SIZE(&context->executors);

  FURLANG_SPARSE_SET_PUT(&context->executors, Furlang_Executor, executor, ((_Furlang_Executor){ .flags = _FURLANG_EXECUTOR_FLAG_RUNNING }));
  return executor;
}

_Furlang_Executor *_furlang_context_get_executor(Furlang_Context context, Furlang_Executor id) {
  assert(context);

  return FURLANG_SPARSE_SET_GET(&context->executors, Furlang_Executor, id);
}

void _furlang_context_remove_executor(Furlang_Context context, Furlang_Executor id) {
  assert(context);
  FURLANG_DA_APPEND(&context->deadExecutors, id);
}