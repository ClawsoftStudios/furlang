#ifndef   __FURLANG_EXECUTOR_H_
#define   __FURLANG_EXECUTOR_H_

#include "furlang/executor.h"
#include "furlang/context.h"

typedef struct _Furlang_Call {
  Furlang_Position position;
} _Furlang_Call;

typedef enum _Furlang_Executor_Flag_Bits {
  _FURLANG_EXECUTOR_FLAG_RUNNING = (1<<0),
  _FURLANG_EXECUTOR_FLAG_PAUSED  = (1<<1),
} _Furlang_Executor_Flag_Bits;
typedef uint8_t _Furlang_Executor_Flags;

struct _Furlang_Executor {
  Furlang_Executor next;
  Furlang_Context context;

  _Furlang_Executor_Flags flags;

  struct {
    _Furlang_Call *items;
    size_t capacity;
    size_t count;
  } callStack;
};

#endif // __FURLANG_EXECUTOR_H_