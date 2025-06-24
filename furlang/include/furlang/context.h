#ifndef   _FURLANG_CONTEXT_H_
#define   _FURLANG_CONTEXT_H_

#include "furlang/alloc/allocator.h"

#include <stddef.h>
#include <stdbool.h>

typedef struct _Furlang_Context *Furlang_Context;

Furlang_Context furlang_context_create(const char *bytecode, size_t bytecodeLength, Furlang_Allocator *allocator);
void furlang_context_destroy(Furlang_Context context, Furlang_Allocator *allocator);

bool furlang_context_is_running(Furlang_Context context);

bool furlang_context_step(Furlang_Context context);
void furlang_context_run(Furlang_Context context);

#endif // _FURLANG_CONTEXT_H_