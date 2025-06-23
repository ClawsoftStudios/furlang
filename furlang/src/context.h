#ifndef   __FURLANG_CONTEXT_H_
#define   __FURLANG_CONTEXT_H_

#include "furlang/context.h"

#include "./executor.h"

#include "furlang/alloc/allocator.h"
#include "furlang/alloc/arena.h"

#include "furlang/utils/sparseSet.h"

FURLANG_SPARSE_SET(_Furlang_Executor_Sparse_Set, _Furlang_Executor, Furlang_Executor, 4);

struct _Furlang_Context {
  const char *bytecode;
  size_t bytecodeLength;

  _Furlang_Executor_Sparse_Set executors;
  struct {
    Furlang_Executor *items;
    size_t capacity;
    size_t count;
  } deadExecutors; // FILO
};

Furlang_Executor _furlang_context_append_executor(Furlang_Context context);
_Furlang_Executor *_furlang_context_get_executor(Furlang_Context context, Furlang_Executor id);
void _furlang_context_remove_executor(Furlang_Context context, Furlang_Executor id);

#endif // __FURLANG_CONTEXT_H_