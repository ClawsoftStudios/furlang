#ifndef   _FURLANG_EXECUTOR_H_
#define   _FURLANG_EXECUTOR_H_

#include <stdint.h>
#include <stdbool.h>

#include "furlang/id.h"

#include "furlang/context.h"

typedef uint64_t Furlang_Position;

typedef Furlang_Id Furlang_Executor;

Furlang_Executor furlang_executor_create(Furlang_Context context, Furlang_Position position);
void furlang_executor_destroy(Furlang_Context context, Furlang_Executor executor);

bool furlang_executor_is_running(Furlang_Context context, Furlang_Executor executor);
bool furlang_executor_is_paused(Furlang_Context context, Furlang_Executor executor);
bool furlang_executor_is_finished(Furlang_Context context, Furlang_Executor executor);

bool furlang_executor_step(Furlang_Context context, Furlang_Executor executor);
void furlang_executor_step_in(Furlang_Context context, Furlang_Executor executor, Furlang_Position position);
bool furlang_executor_step_out(Furlang_Context context, Furlang_Executor executor);

#endif // _FURLANG_EXECUTOR_H_