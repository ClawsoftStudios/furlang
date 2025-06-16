#include "./executor.h"

#include "./context.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>

Furlang_Executor furlang_executor_create(Furlang_Context context, Furlang_Position position) {
  (void)position;

  assert(context);

  Furlang_Executor executor = furlang_allocator_alloc(&context->executorAllocator, sizeof(*executor));
  assert(executor);

  memset(executor, 0, sizeof(*executor));
  executor->context = context;
  executor->flags = _FURLANG_EXECUTOR_FLAG_RUNNING;

  if (!context->executors.head) context->executors.head = executor;
  else context->executors.tail->next = executor;
  context->executors.tail = executor;

  return executor;
}

void furlang_executor_destroy(Furlang_Executor executor) {
  assert(executor);
  free(executor->callStack.items);
  furlang_allocator_free(&executor->context->executorAllocator, executor);
}

bool furlang_executor_is_running(const Furlang_Executor executor) {
  assert(executor);
  return (executor->flags & _FURLANG_EXECUTOR_FLAG_RUNNING) && !(executor->flags & _FURLANG_EXECUTOR_FLAG_PAUSED);
}

bool furlang_executor_is_paused(const Furlang_Executor executor) {
  assert(executor);
  return executor->flags & _FURLANG_EXECUTOR_FLAG_PAUSED;
}

bool furlang_executor_is_finished(const Furlang_Executor executor) {
  assert(executor);
  return !(executor->flags & _FURLANG_EXECUTOR_FLAG_RUNNING);
}

bool furlang_executor_step(Furlang_Executor executor) {
  if (!furlang_executor_is_running(executor)) return false;

  return true;
}

void furlang_executor_step_in(Furlang_Executor executor, Furlang_Position position) {
  (void)position;

  assert(executor);


}

bool furlang_executor_step_out(Furlang_Executor executor) {
  if (furlang_executor_is_finished(executor)) return false;

  return true;
}