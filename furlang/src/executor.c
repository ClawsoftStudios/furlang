#include "./executor.h"

#include "./context.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>

Furlang_Executor furlang_executor_create(Furlang_Context context, Furlang_Module module, size_t function) {
  assert(context);
  return _furlang_context_append_executor(context, (Furlang_Position){ .module = module, .address = FURLANG_DA_AT(&FURLANG_DA_AT(&context->modules, module).functions, function).address });
}

void furlang_executor_destroy(Furlang_Context context, Furlang_Executor executor) {
  assert(context);
  _furlang_executor_cleanup(context, _furlang_context_get_executor(context, executor));
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
  _Furlang_Module *module = _furlang_executor_get_module(context, e);
  Furlang_Instruction instruction = _furlang_executor_get_instruction(e, module);

  switch (instruction) {
  case FURLANG_INSTRUCTION_NOP: break;
  case FURLANG_INSTRUCTION_IPUSH: {
    Furlang_Thing thing = _furlang_context_append_thing(context, FURLANG_THING_TYPE_INT);
    _Furlang_Thing *t = _furlang_context_get_thing(context, thing);

    *(Furlang_Int*)t->data = _furlang_executor_get_int(e, module);

    _furlang_executor_push(context, e, thing);
  } break;
  case FURLANG_INSTRUCTION_BIPUSH: {
    Furlang_Thing thing = _furlang_context_append_thing(context, FURLANG_THING_TYPE_INT);
    _Furlang_Thing *t = _furlang_context_get_thing(context, thing);

    *(Furlang_Int*)t->data = _furlang_executor_get_byte(e, module);

    _furlang_executor_push(context, e, thing);
  } break;
  case FURLANG_INSTRUCTION_FPUSH: {
    Furlang_Thing thing = _furlang_context_append_thing(context, FURLANG_THING_TYPE_FUNCTION);
    _Furlang_Thing *t = _furlang_context_get_thing(context, thing);

    *(Furlang_Function*)t->data = _furlang_executor_get_ulong(e, module);

    _furlang_executor_push(context, e, thing);
  } break;
  case FURLANG_INSTRUCTION_SFPUSH: {
    Furlang_Thing thing = _furlang_context_append_thing(context, FURLANG_THING_TYPE_FUNCTION);
    _Furlang_Thing *t = _furlang_context_get_thing(context, thing);

    *(Furlang_Function*)t->data = _furlang_executor_get_ushort(e, module);

    _furlang_executor_push(context, e, thing);
  } break;
  case FURLANG_INSTRUCTION_DUP: {
    Furlang_Thing thing = _furlang_executor_peek(e);
    _furlang_executor_push(context, e, furlang_thing_clone(context, thing));
  } break;
  case FURLANG_INSTRUCTION_SWAP: {
    Furlang_Thing b = _furlang_executor_pop(context, e);
    Furlang_Thing a = _furlang_executor_pop(context, e);
    _furlang_executor_push(context, e, b);
    _furlang_executor_push(context, e, a);
  } break;
  case FURLANG_INSTRUCTION_POP: {
    (void)_furlang_executor_pop(context, e);
  } break;
  case FURLANG_INSTRUCTION_RET: {
    return furlang_executor_step_out(context, executor);
  } break;
  case FURLANG_INSTRUCTION_STR: {
    _furlang_executor_store_variable(context, e, _furlang_executor_get_ushort(e, module), _furlang_executor_pop(context, e));
  } break;
  case FURLANG_INSTRUCTION_LOD: {
    _furlang_executor_push(context, e, _furlang_executor_load_variable(e, _furlang_executor_get_ushort(e, module)));
  } break;
  case FURLANG_INSTRUCTION_STRG: {
    _furlang_module_store_global_var(context, module, _furlang_executor_get_ushort(e, module), _furlang_executor_pop(context, e));
  } break;
  case FURLANG_INSTRUCTION_LODG: {
    _furlang_executor_push(context, e, _furlang_module_load_global_var(context, module, _furlang_executor_get_ushort(e, module)));
  } break;
  case FURLANG_INSTRUCTION_JMP: {
    Furlang_Addr addr = _furlang_executor_get_addr(e, module);
    assert(addr < module->bytecodeLength);
    FURLANG_DA_BACK(&e->callStack).position.address = addr;
  } break;
  case FURLANG_INSTRUCTION_JZ: {
    Furlang_Addr addr = _furlang_executor_get_addr(e, module);
    assert(addr < module->bytecodeLength);
    if (*(Furlang_Int*)_furlang_context_get_thing(context, _furlang_executor_pop(context, e))->data == 0) FURLANG_DA_BACK(&e->callStack).position.address = addr;
  } break;
  case FURLANG_INSTRUCTION_JNZ: {
    Furlang_Addr addr = _furlang_executor_get_addr(e, module);
    assert(addr < module->bytecodeLength);
    if (*(Furlang_Int*)_furlang_context_get_thing(context, _furlang_executor_pop(context, e))->data != 0) FURLANG_DA_BACK(&e->callStack).position.address = addr;
  } break;
  case FURLANG_INSTRUCTION_CALL: {
    Furlang_Thing thing = _furlang_executor_pop(context, e);
    _Furlang_Thing *t = _furlang_context_get_thing(context, thing);
    assert(t->type == FURLANG_THING_TYPE_FUNCTION);

    Furlang_Module_Function function = FURLANG_DA_AT(&module->functions, *(Furlang_Function*)t->data);

    Furlang_Thing *params = malloc(sizeof(*params) * function.paramCount);
    assert(params);

    for (size_t i = 0; i < function.paramCount; ++i) params[function.paramCount-i-1] = _furlang_executor_pop(context, e);
    furlang_executor_step_in(context, executor, (Furlang_Position){ .module = FURLANG_DA_BACK(&e->callStack).position.module, .address = function.address });
    for (size_t i = 0; i < function.paramCount; ++i) _furlang_executor_store_variable(context, e, i, params[i]);

    free(params);
  } break;

  #define BINOP(op)                                                                               \
    {                                                                                             \
      _Furlang_Thing *b = _furlang_context_get_thing(context, _furlang_executor_pop(context, e)); \
      _Furlang_Thing *a = _furlang_context_get_thing(context, _furlang_executor_pop(context, e)); \
                                                                                                  \
      _Furlang_Thing_Op_Func func = _furlang_thing_ops[_FURLANG_THING_OP(a->type, b->type)][op];  \
      assert(func);                                                                               \
      _furlang_executor_push(context, e, func(context, a, b));                                    \
    } break

  case FURLANG_INSTRUCTION_ADD: BINOP(_FURLANG_THING_OP_ADD);
  case FURLANG_INSTRUCTION_SUB: BINOP(_FURLANG_THING_OP_SUB);
  case FURLANG_INSTRUCTION_MUL: BINOP(_FURLANG_THING_OP_MUL);
  case FURLANG_INSTRUCTION_DIV: BINOP(_FURLANG_THING_OP_DIV);
  case FURLANG_INSTRUCTION_MOD: BINOP(_FURLANG_THING_OP_MOD);

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
  _furlang_executor_push_scope(e);
}

bool furlang_executor_step_out(Furlang_Context context, Furlang_Executor executor) {
  assert(context);
  _Furlang_Executor *e = _furlang_context_get_executor(context, executor);
  FURLANG_DA_POP_BACK(&e->callStack);
  _furlang_executor_pop_scope(context, e);

  if (e->callStack.count) return true;
  return (e->flags = 0, false);
}



void _furlang_executor_cleanup(Furlang_Context context, _Furlang_Executor *e) {
  assert(e);

  while (e->scopes.count) _furlang_executor_pop_scope(context, e);
  FURLANG_DA_FREE(&e->scopes);

  FURLANG_DA_FREE(&e->callStack);

  memset(e, 0, sizeof(*e));
}

_Furlang_Module *_furlang_executor_get_module(Furlang_Context context, _Furlang_Executor *e) {
  assert(context);
  assert(e);
  return &FURLANG_DA_AT(&context->modules, FURLANG_DA_BACK(&e->callStack).position.module);
}

Furlang_Instruction _furlang_executor_get_instruction(_Furlang_Executor *e, _Furlang_Module *module) {
  assert(e->flags & _FURLANG_EXECUTOR_FLAG_RUNNING);
  assert(e->callStack.count);
  assert(module);

  _Furlang_Call *topCall = &FURLANG_DA_BACK(&e->callStack);
  assert(topCall->position.address < module->bytecodeLength);
  return module->bytecode[topCall->position.address++];
}

Furlang_Byte _furlang_executor_get_byte(_Furlang_Executor *e, _Furlang_Module *module) {
  assert(e->flags & _FURLANG_EXECUTOR_FLAG_RUNNING);
  assert(e->callStack.count);
  assert(module);

  _Furlang_Call *topCall = &e->callStack.items[e->callStack.count-1];
  assert(topCall->position.address < module->bytecodeLength);
  return module->bytecode[topCall->position.address++];
}

Furlang_Int _furlang_executor_get_int(_Furlang_Executor *e, _Furlang_Module *module) {
  assert(e->flags & _FURLANG_EXECUTOR_FLAG_RUNNING);
  assert(e->callStack.count);
  assert(module);

  _Furlang_Call *topCall = &e->callStack.items[e->callStack.count-1];
  assert(topCall->position.address+4 <= module->bytecodeLength);
  Furlang_Int value = (module->bytecode[topCall->position.address+0] << 8*3) | (module->bytecode[topCall->position.address+1] << 8*2) | (module->bytecode[topCall->position.address+2] << 8*1) | (module->bytecode[topCall->position.address+3] << 8*0);
  topCall->position.address += 4;
  return value;
}

Furlang_Addr _furlang_executor_get_addr(_Furlang_Executor *e, _Furlang_Module *module) {
  assert(e->flags & _FURLANG_EXECUTOR_FLAG_RUNNING);
  assert(e->callStack.count);
  assert(module);

  _Furlang_Call *topCall = &e->callStack.items[e->callStack.count-1];
  assert(topCall->position.address+8 <= module->bytecodeLength);
  Furlang_Addr value = ((Furlang_Addr)module->bytecode[topCall->position.address+0] << 8*7) | ((Furlang_Addr)module->bytecode[topCall->position.address+1] << 8*6)
                     | ((Furlang_Addr)module->bytecode[topCall->position.address+2] << 8*5) | ((Furlang_Addr)module->bytecode[topCall->position.address+3] << 8*4)
                     | ((Furlang_Addr)module->bytecode[topCall->position.address+4] << 8*3) | ((Furlang_Addr)module->bytecode[topCall->position.address+5] << 8*2)
                     | ((Furlang_Addr)module->bytecode[topCall->position.address+6] << 8*1) | ((Furlang_Addr)module->bytecode[topCall->position.address+7] << 8*0);
  topCall->position.address += 8;
  return value;
}

uint16_t _furlang_executor_get_ushort(_Furlang_Executor *e, _Furlang_Module *module) {
  assert(e->flags & _FURLANG_EXECUTOR_FLAG_RUNNING);
  assert(e->callStack.count);
  assert(module);

  _Furlang_Call *topCall = &e->callStack.items[e->callStack.count-1];
  assert(topCall->position.address+2 <= module->bytecodeLength);
  uint16_t value = (module->bytecode[topCall->position.address+0] << 8) | (module->bytecode[topCall->position.address+1]);
  topCall->position.address += 2;
  return value;
}

uint64_t _furlang_executor_get_ulong(_Furlang_Executor *e, _Furlang_Module *module) {
  assert(e->flags & _FURLANG_EXECUTOR_FLAG_RUNNING);
  assert(e->callStack.count);
  assert(module);

  _Furlang_Call *topCall = &e->callStack.items[e->callStack.count-1];
  assert(topCall->position.address+8 <= module->bytecodeLength);
  uint64_t value = ((uint64_t)module->bytecode[topCall->position.address+0] << 8*7) | ((uint64_t)module->bytecode[topCall->position.address+1] << 8*6)
                 | ((uint64_t)module->bytecode[topCall->position.address+2] << 8*5) | ((uint64_t)module->bytecode[topCall->position.address+3] << 8*4)
                 | ((uint64_t)module->bytecode[topCall->position.address+4] << 8*3) | ((uint64_t)module->bytecode[topCall->position.address+5] << 8*2)
                 | ((uint64_t)module->bytecode[topCall->position.address+6] << 8*1) | ((uint64_t)module->bytecode[topCall->position.address+7] << 8*0);
  topCall->position.address += 8;
  return value;
}

void _furlang_executor_push(Furlang_Context context, _Furlang_Executor *e, Furlang_Thing thing) {
  assert(e);
  assert(thing != (Furlang_Thing)-1);

  _Furlang_Scope *scope = &FURLANG_DA_BACK(&e->scopes);
  FURLANG_DA_APPEND(&scope->thingStack, thing);
  furlang_thing_reference(context, thing);
}

Furlang_Thing _furlang_executor_pop(Furlang_Context context, _Furlang_Executor *e) {
  assert(e);

  _Furlang_Scope *scope = &FURLANG_DA_BACK(&e->scopes);
  assert(scope->thingStack.count);

  Furlang_Thing thing = FURLANG_DA_POP_BACK(&scope->thingStack);
  furlang_thing_unreference(context, thing);
  return thing;
}

Furlang_Thing _furlang_executor_peek(_Furlang_Executor *e) {
  assert(e);

  _Furlang_Scope *scope = &FURLANG_DA_BACK(&e->scopes);
  assert(scope->thingStack.count);
  return FURLANG_DA_BACK(&scope->thingStack);
}

void _furlang_executor_push_scope(_Furlang_Executor *e) {
  assert(e);
  assert(e->flags & _FURLANG_EXECUTOR_FLAG_RUNNING);
  FURLANG_DA_APPEND(&e->scopes, ((_Furlang_Scope){0}));
}

void _furlang_executor_pop_scope(Furlang_Context context, _Furlang_Executor *e) {
  assert(e);
  _Furlang_Scope scope = FURLANG_DA_POP_BACK(&e->scopes);

  while (scope.variables.count) furlang_thing_unreference(context, FURLANG_DA_POP_BACK(&scope.variables));
  FURLANG_DA_FREE(&scope.variables);

  while (scope.thingStack.count) furlang_thing_unreference(context, FURLANG_DA_POP_BACK(&scope.thingStack));
  FURLANG_DA_FREE(&scope.thingStack);
}

void _furlang_executor_store_variable(Furlang_Context context, _Furlang_Executor *e, uint16_t index, Furlang_Thing thing) {
  assert(e);
  assert(e->scopes.count);

  _Furlang_Scope *scope = &FURLANG_DA_BACK(&e->scopes);

  size_t i = scope->variables.count;
  FURLANG_DA_RESIZE(&scope->variables, index+1);
  for (; i < scope->variables.count; ++i) FURLANG_DA_AT(&scope->variables, i) = _FURLANG_DEAD_THING;

  if (FURLANG_DA_AT(&scope->variables, index) != _FURLANG_DEAD_THING) furlang_thing_unreference(context, FURLANG_DA_AT(&scope->variables, index));
  FURLANG_DA_AT(&scope->variables, index) = thing;
  furlang_thing_reference(context, thing);
}

Furlang_Thing _furlang_executor_load_variable(_Furlang_Executor *e, uint16_t index) {
  assert(e);
  assert(e->scopes.count);

  _Furlang_Scope *scope = &FURLANG_DA_AT(&e->scopes, e->scopes.count-1);
  assert(index < scope->variables.count);
  return FURLANG_DA_AT(&scope->variables, index);
}