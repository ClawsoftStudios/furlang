#include "./context.h"

#include "./executor.h"

#include <string.h>
#include <assert.h>

Furlang_Context furlang_context_create(Furlang_Allocator *allocator) {
  Furlang_Context context = furlang_allocator_alloc(allocator, sizeof(*context));
  assert(context);

  memset(context, 0, sizeof(*context));
  context->executorAllocator = (Furlang_Allocator){
    .data    = &context->executorArena,
    .alloc   = (Furlang_Allocator_Alloc)furlang_arena_alloc,
    .free    = NULL,
    .realloc = NULL,
  };

  return context;
}

void furlang_context_destroy(Furlang_Context context, Furlang_Allocator *allocator) {
  for (Furlang_Executor e = context->executors.head; e->next; e = e->next)
    furlang_executor_destroy(e);
  furlang_arena_free(&context->executorArena);

  furlang_allocator_free(allocator, context);
}

bool furlang_context_is_running(Furlang_Context context) {
  for (Furlang_Executor e = context->executors.head; e->next; e = e->next)
    if (furlang_executor_is_running(e)) return true;
  return false;
}

bool furlang_context_step(Furlang_Context context) {
  bool ran = false;
  for (Furlang_Executor e = context->executors.head; e->next; e = e->next)
    if (furlang_executor_step(e)) ran = true;

  return ran;
}

void furlang_context_run(Furlang_Context context) {
  while (furlang_context_step(context));
}