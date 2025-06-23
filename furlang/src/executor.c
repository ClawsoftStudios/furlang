#include "./executor.h"

#include "./context.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>

Furlang_Executor furlang_executor_create(Furlang_Context context, Furlang_Position position) {
  (void)position;
  assert(context);
  return _furlang_context_append_executor(context);
}

void furlang_executor_destroy(Furlang_Context context, Furlang_Executor executor) {
  assert(context);
  _furlang_context_remove_executor(context, executor);
}

bool furlang_executor_is_running(Furlang_Context context, Furlang_Executor executor) {
  assert(context);
  _Furlang_Executor *ex = _furlang_context_get_executor(context, executor);
  return (ex->flags & _FURLANG_EXECUTOR_FLAG_RUNNING) && !(ex->flags & _FURLANG_EXECUTOR_FLAG_PAUSED);
}

bool furlang_executor_is_paused(Furlang_Context context, Furlang_Executor executor) {
  assert(context);
  _Furlang_Executor *ex = _furlang_context_get_executor(context, executor);
  return ex->flags & _FURLANG_EXECUTOR_FLAG_PAUSED;
}

bool furlang_executor_is_finished(Furlang_Context context, Furlang_Executor executor) {
  assert(context);
  _Furlang_Executor *ex = _furlang_context_get_executor(context, executor);
  return !(ex->flags & _FURLANG_EXECUTOR_FLAG_RUNNING);
}

bool furlang_executor_step(Furlang_Context context, Furlang_Executor executor) {
  assert(context);
  _Furlang_Executor *ex = _furlang_context_get_executor(context, executor);
  (void)ex;
  return false;
}

void furlang_executor_step_in(Furlang_Context context, Furlang_Executor executor, Furlang_Position position) {
  assert(context);
  _Furlang_Executor *ex = _furlang_context_get_executor(context, executor);
  (void)ex;
  (void)position;
}

bool furlang_executor_step_out(Furlang_Context context, Furlang_Executor executor) {
  assert(context);
  _Furlang_Executor *ex = _furlang_context_get_executor(context, executor);
  (void)ex;
  return false;
}