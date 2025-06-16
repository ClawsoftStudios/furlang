#ifndef   _FURLANG_EXECUTOR_H_
#define   _FURLANG_EXECUTOR_H_

#include <stdint.h>
#include <stdbool.h>

#include "furlang/context.h"

typedef uint64_t Furlang_Position;

typedef struct _Furlang_Executor *Furlang_Executor;

Furlang_Executor furlang_executor_create(Furlang_Context context, Furlang_Position position);
void furlang_executor_destroy(Furlang_Executor executor);

bool furlang_executor_is_running(const Furlang_Executor executor);
bool furlang_executor_is_paused(const Furlang_Executor executor);
bool furlang_executor_is_finished(const Furlang_Executor executor);

bool furlang_executor_step(Furlang_Executor executor);
void furlang_executor_step_in(Furlang_Executor executor, Furlang_Position position);
bool furlang_executor_step_out(Furlang_Executor executor);

#endif // _FURLANG_EXECUTOR_H_