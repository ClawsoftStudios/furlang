#ifndef   __FURLANG_EXECUTOR_H_
#define   __FURLANG_EXECUTOR_H_

#include "furlang/executor.h"
#include "furlang/thing.h"
#include "furlang/context.h"

#include "furlang/bytecode.h"

typedef struct _Furlang_Call {
  Furlang_Position position;
} _Furlang_Call;

typedef enum _Furlang_Executor_Flag_Bits {
  _FURLANG_EXECUTOR_FLAG_RUNNING = (1<<0),
  _FURLANG_EXECUTOR_FLAG_PAUSED  = (1<<1),
} _Furlang_Executor_Flag_Bits;
typedef uint8_t _Furlang_Executor_Flags;

typedef struct _Furlang_Executor {
  _Furlang_Executor_Flags flags;

  struct {
    _Furlang_Call *items;
    size_t capacity;
    size_t count;
  } callStack;

  struct {
    Furlang_Thing *items;
    size_t capacity;
    size_t count;
  } thingStack;
} _Furlang_Executor;

void _furlang_executor_cleanup(_Furlang_Executor *e);

Furlang_Instruction _furlang_executor_get_instruction(Furlang_Context context, _Furlang_Executor *e);
Furlang_Byte _furlang_executor_get_byte(Furlang_Context context, _Furlang_Executor *e);
Furlang_Int _furlang_executor_get_int(Furlang_Context context, _Furlang_Executor *e);

void _furlang_executor_push(Furlang_Context context, _Furlang_Executor *e, Furlang_Thing thing);
Furlang_Thing _furlang_executor_pop(Furlang_Context context, _Furlang_Executor *e);
Furlang_Thing _furlang_executor_peek(_Furlang_Executor *e);

#endif // __FURLANG_EXECUTOR_H_