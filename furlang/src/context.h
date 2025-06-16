#ifndef   __FURLANG_CONTEXT_H_
#define   __FURLANG_CONTEXT_H_

#include "furlang/context.h"
#include "furlang/executor.h"

#include "furlang/alloc/allocator.h"
#include "furlang/alloc/arena.h"

struct _Furlang_Context {
  const char *bytecode;
  size_t bytecodeLength;

  struct {
    Furlang_Executor head, tail;
  } executors;

  Furlang_Arena executorArena;
  Furlang_Allocator executorAllocator;
};

#endif // __FURLANG_CONTEXT_H_