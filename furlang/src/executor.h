#ifndef   __FURLANG_EXECUTOR_H_
#define   __FURLANG_EXECUTOR_H_

#include "furlang/executor.h"

#include "furlang/thing.h"
#include "furlang/context.h"
#include "./module.h"

#include "furlang/bytecode.h"

typedef struct _Furlang_Call {
  Furlang_Position position;
} _Furlang_Call;

typedef struct _Furlang_Scope {
  struct {
    Furlang_Thing *items;
    uint16_t capacity;
    uint16_t count;
  } variables;

  struct {
    Furlang_Thing *items;
    size_t capacity;
    size_t count;
  } thingStack;
} _Furlang_Scope;

typedef enum _Furlang_Executor_Flag_Bits {
  _FURLANG_EXECUTOR_FLAG_RUNNING = (1<<0),
  _FURLANG_EXECUTOR_FLAG_PAUSED  = (1<<1),
} _Furlang_Executor_Flag_Bits;
typedef uint8_t _Furlang_Executor_Flags;

typedef struct _Furlang_Executor {
  _Furlang_Executor_Flags flags;

  struct {
    _Furlang_Scope *items;
    size_t capacity;
    size_t count;
  } scopes;

  struct {
    _Furlang_Call *items;
    size_t capacity;
    size_t count;
  } callStack;
} _Furlang_Executor;

void _furlang_executor_cleanup(Furlang_Context context, _Furlang_Executor *e);

_Furlang_Module *_furlang_executor_get_module(Furlang_Context context, _Furlang_Executor *e);

Furlang_Instruction _furlang_executor_get_instruction(_Furlang_Executor *e, _Furlang_Module *module);
Furlang_Byte _furlang_executor_get_byte(_Furlang_Executor *e, _Furlang_Module *module);
Furlang_Int _furlang_executor_get_int(_Furlang_Executor *e, _Furlang_Module *module);
Furlang_Addr _furlang_executor_get_addr(_Furlang_Executor *e, _Furlang_Module *module);
uint16_t _furlang_executor_get_ushort(_Furlang_Executor *e, _Furlang_Module *module);
uint64_t _furlang_executor_get_ulong(_Furlang_Executor *e, _Furlang_Module *module);

void _furlang_executor_push(Furlang_Context context, _Furlang_Executor *e, Furlang_Thing thing);
Furlang_Thing _furlang_executor_pop(Furlang_Context context, _Furlang_Executor *e);
Furlang_Thing _furlang_executor_peek(_Furlang_Executor *e);

void _furlang_executor_push_scope(_Furlang_Executor *e);
void _furlang_executor_pop_scope(Furlang_Context context, _Furlang_Executor *e);

void _furlang_executor_store_variable(Furlang_Context context, _Furlang_Executor *e, uint16_t index, Furlang_Thing thing);
Furlang_Thing _furlang_executor_load_variable(_Furlang_Executor *e, uint16_t index);

#endif // __FURLANG_EXECUTOR_H_