#include "./module.h"

#include "./context.h"

#include <furlang/utils/da.h>

#include <string.h>
#include <stdio.h>
#include <assert.h>

Furlang_Module furlang_module_create(Furlang_Context context, Furlang_Module_Info info) {
  Furlang_Module module = context->modules.count;

  _Furlang_Module m = {
    .bytecodeLength = info.bytecodeLength,
    .bytecode = malloc(info.bytecodeLength)
  };
  assert(m.bytecode);
  memcpy(m.bytecode, info.bytecode, info.bytecodeLength);

  FURLANG_DA_RESIZE(&m.functions, info.functionCount);
  if (info.functionCount) memcpy(m.functions.items, info.functions, sizeof(*info.functions) * info.functionCount);
  m.globalVariables.items = malloc(sizeof(*m.globalVariables.items) * (m.globalVariables.count = info.globalVariableCount));
  assert(m.globalVariables.items);

  FURLANG_DA_APPEND(&context->modules, m);

  return module;
}

size_t furlang_module_add_function(Furlang_Context context, Furlang_Module module, Furlang_Module_Function function) {
  assert(module);

  size_t index = FURLANG_DA_AT(&context->modules, module).functions.count;
  FURLANG_DA_APPEND(&FURLANG_DA_AT(&context->modules, module).functions, function);
  return index;
}

Furlang_Module_Function furlang_module_get_function(Furlang_Context context, Furlang_Module module, size_t index) {
  assert(module);

  return FURLANG_DA_AT(&FURLANG_DA_AT(&context->modules, module).functions, index);
}

void _furlang_module_store_global_var(Furlang_Context context, _Furlang_Module *module, uint16_t index, Furlang_Thing thing) {
  assert(context);
  assert(module);
  assert(index < module->globalVariables.count);

  if (module->globalVariables.items[index] != _FURLANG_DEAD_THING) furlang_thing_unreference(context, module->globalVariables.items[index]);
  module->globalVariables.items[index] = thing;
  furlang_thing_reference(context, thing);
}

Furlang_Thing _furlang_module_load_global_var(Furlang_Context context, _Furlang_Module *module, uint16_t index) {
  assert(context);
  assert(module);
  assert(index < module->globalVariables.count);
  return module->globalVariables.items[index];
}