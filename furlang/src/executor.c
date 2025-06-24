#include "./executor.h"

#include "./context.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>

Furlang_Executor furlang_executor_create(Furlang_Context context, Furlang_Position position) {
  assert(context);

  return _furlang_context_append_executor(context, position);
}

void furlang_executor_destroy(Furlang_Context context, Furlang_Executor executor) {
  assert(context);
  _furlang_executor_cleanup(_furlang_context_get_executor(context, executor));
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
  if (furlang_executor_is_paused(context, executor)) return false;

  _Furlang_Executor *e = _furlang_context_get_executor(context, executor);
  Furlang_Instruction instruction = _furlang_executor_get_instruction(context, e);

  switch (instruction) {
  case FURLANG_INSTRUCTION_NOP: break;
  case FURLANG_INSTRUCTION_IPUSH: {
    Furlang_Thing thing = _furlang_context_append_thing(context, FURLANG_THING_TYPE_INT);
    _Furlang_Thing *t = _furlang_context_get_thing(context, thing);

    *(Furlang_Int*)t->data = _furlang_executor_get_int(context, e);

    _furlang_executor_push(context, e, thing);
  } break;
  case FURLANG_INSTRUCTION_BIPUSH: {
    Furlang_Thing thing = _furlang_context_append_thing(context, FURLANG_THING_TYPE_INT);
    _Furlang_Thing *t = _furlang_context_get_thing(context, thing);

    *(Furlang_Int*)t->data = _furlang_executor_get_byte(context, e);

    _furlang_executor_push(context, e, thing);
  } break;
  case FURLANG_INSTRUCTION_DUP: {
    Furlang_Thing thing = _furlang_executor_peek(e);
    _furlang_executor_push(context, e, furlang_thing_clone(context, thing));
  } break;
  case FURLANG_INSTRUCTION_SWAP: {
    Furlang_Thing a = _furlang_executor_pop(context, e);
    Furlang_Thing b = _furlang_executor_pop(context, e);
    _furlang_executor_push(context, e, b);
    _furlang_executor_push(context, e, a);
  } break;
  case FURLANG_INSTRUCTION_POP: {
    (void)_furlang_executor_pop(context, e);
  } break;
  case FURLANG_INSTRUCTION_RET: {
    return furlang_executor_step_out(context, executor);
  } break;
  case FURLANG_INSTRUCTION_ADD: {
    Furlang_Thing a = _furlang_executor_pop(context, e);
    Furlang_Thing b = _furlang_executor_pop(context, e);

    _Furlang_Thing *at = _furlang_context_get_thing(context, a);
    _Furlang_Thing *bt = _furlang_context_get_thing(context, b);

    Furlang_Int av = *(Furlang_Int*)at->data;
    Furlang_Int bv = *(Furlang_Int*)bt->data;

    Furlang_Thing c = furlang_thing_create(context, FURLANG_THING_TYPE_INT);
    _Furlang_Thing *ct = _furlang_context_get_thing(context, c);
    _furlang_executor_push(context, e, c);

    *(Furlang_Int*)ct->data = av + bv;
  } break;
  case FURLANG_INSTRUCTION_SUB: {
    Furlang_Thing a = _furlang_executor_pop(context, e);
    Furlang_Thing b = _furlang_executor_pop(context, e);

    _Furlang_Thing *at = _furlang_context_get_thing(context, a);
    _Furlang_Thing *bt = _furlang_context_get_thing(context, b);

    Furlang_Int av = *(Furlang_Int*)at->data;
    Furlang_Int bv = *(Furlang_Int*)bt->data;

    Furlang_Thing c = furlang_thing_create(context, FURLANG_THING_TYPE_INT);
    _Furlang_Thing *ct = _furlang_context_get_thing(context, c);
    _furlang_executor_push(context, e, c);

    *(Furlang_Int*)ct->data = av - bv;
  } break;
  case FURLANG_INSTRUCTION_MUL: {
    Furlang_Thing a = _furlang_executor_pop(context, e);
    Furlang_Thing b = _furlang_executor_pop(context, e);

    _Furlang_Thing *at = _furlang_context_get_thing(context, a);
    _Furlang_Thing *bt = _furlang_context_get_thing(context, b);

    Furlang_Int av = *(Furlang_Int*)at->data;
    Furlang_Int bv = *(Furlang_Int*)bt->data;

    Furlang_Thing c = furlang_thing_create(context, FURLANG_THING_TYPE_INT);
    _Furlang_Thing *ct = _furlang_context_get_thing(context, c);
    _furlang_executor_push(context, e, c);

    *(Furlang_Int*)ct->data = av * bv;
  } break;
  case FURLANG_INSTRUCTION_DIV: {
    Furlang_Thing a = _furlang_executor_pop(context, e);
    Furlang_Thing b = _furlang_executor_pop(context, e);

    _Furlang_Thing *at = _furlang_context_get_thing(context, a);
    _Furlang_Thing *bt = _furlang_context_get_thing(context, b);

    Furlang_Int av = *(Furlang_Int*)at->data;
    Furlang_Int bv = *(Furlang_Int*)bt->data;

    Furlang_Thing c = furlang_thing_create(context, FURLANG_THING_TYPE_INT);
    _Furlang_Thing *ct = _furlang_context_get_thing(context, c);
    _furlang_executor_push(context, e, c);

    *(Furlang_Int*)ct->data = av / bv;
  } break;
  case FURLANG_INSTRUCTION_MOD: {
    Furlang_Thing a = _furlang_executor_pop(context, e);
    Furlang_Thing b = _furlang_executor_pop(context, e);

    _Furlang_Thing *at = _furlang_context_get_thing(context, a);
    _Furlang_Thing *bt = _furlang_context_get_thing(context, b);

    Furlang_Int av = *(Furlang_Int*)at->data;
    Furlang_Int bv = *(Furlang_Int*)bt->data;

    Furlang_Thing c = furlang_thing_create(context, FURLANG_THING_TYPE_INT);
    _Furlang_Thing *ct = _furlang_context_get_thing(context, c);
    _furlang_executor_push(context, e, c);

    *(Furlang_Int*)ct->data = av % bv;
  } break;

  case COUNT_FURLANG_INSTRUCTIONS:
  default: assert(0);
  }

  return true;
}

void furlang_executor_step_in(Furlang_Context context, Furlang_Executor executor, Furlang_Position position) {
  assert(context);

  _Furlang_Executor *e = _furlang_context_get_executor(context, executor);
  FURLANG_DA_APPEND(&e->callStack, ((_Furlang_Call){
    .position = position
  }));
}

bool furlang_executor_step_out(Furlang_Context context, Furlang_Executor executor) {
  assert(context);
  _Furlang_Executor *e = _furlang_context_get_executor(context, executor);
  FURLANG_DA_POP_BACK(&e->callStack);

  if (e->callStack.count) return true;
  return (e->flags = 0, false);
}



void _furlang_executor_cleanup(_Furlang_Executor *e) {
  assert(e);
  FURLANG_DA_FREE(&e->callStack);
  FURLANG_DA_FREE(&e->thingStack);

  memset(e, 0, sizeof(*e));
}

Furlang_Instruction _furlang_executor_get_instruction(Furlang_Context context, _Furlang_Executor *e) {
  assert(context);
  assert(e->flags & _FURLANG_EXECUTOR_FLAG_RUNNING);
  assert(e->callStack.count);

  _Furlang_Call *topCall = &e->callStack.items[e->callStack.count-1];
  assert(topCall->position < context->bytecodeLength);
  return context->bytecode[topCall->position++];
}

Furlang_Byte _furlang_executor_get_byte(Furlang_Context context, _Furlang_Executor *e) {
  assert(context);
  assert(e->flags & _FURLANG_EXECUTOR_FLAG_RUNNING);
  assert(e->callStack.count);

  _Furlang_Call *topCall = &e->callStack.items[e->callStack.count-1];
  assert(topCall->position < context->bytecodeLength);
  return context->bytecode[topCall->position++];
}

Furlang_Int _furlang_executor_get_int(Furlang_Context context, _Furlang_Executor *e) {
  assert(context);
  assert(e->flags & _FURLANG_EXECUTOR_FLAG_RUNNING);
  assert(e->callStack.count);

  _Furlang_Call *topCall = &e->callStack.items[e->callStack.count-1];
  assert(topCall->position+4 <= context->bytecodeLength);
  Furlang_Int value = (context->bytecode[topCall->position+0] << 8*3) | (context->bytecode[topCall->position+1] << 8*2) | (context->bytecode[topCall->position+2] << 8*1) | (context->bytecode[topCall->position+3] << 8*0);
  topCall->position += 4;
  return value;
}

void _furlang_executor_push(Furlang_Context context, _Furlang_Executor *e, Furlang_Thing thing) {
  assert(e);
  assert(thing != (Furlang_Thing)-1);
  furlang_thing_reference(context, thing);
  FURLANG_DA_APPEND(&e->thingStack, thing);
}

Furlang_Thing _furlang_executor_pop(Furlang_Context context, _Furlang_Executor *e) {
  assert(e);
  assert(e->thingStack.count);
  Furlang_Thing thing = FURLANG_DA_POP_BACK(&e->thingStack);
  furlang_thing_unreference(context, thing);
  return thing;
}

Furlang_Thing _furlang_executor_peek(_Furlang_Executor *e) {
  assert(e);
  assert(e->thingStack.count);
  return e->thingStack.items[e->thingStack.count-1];
}