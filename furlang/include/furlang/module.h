#ifndef   _FURLANG_MODULE_H_
#define   _FURLANG_MODULE_H_

#include "furlang/bytecode.h"
#include "furlang/context.h"
#include "furlang/alloc/allocator.h"

#include <stddef.h>

typedef size_t Furlang_Module;

typedef struct Furlang_Module_Function {
  Furlang_Addr address;
  uint16_t paramCount;
} Furlang_Module_Function;

typedef struct Furlang_Module_Info {
  size_t functionCount;
  const Furlang_Module_Function *functions;
  size_t globalVariableCount;

  size_t bytecodeLength;
  const char *bytecode;
} Furlang_Module_Info;

Furlang_Module furlang_module_create(Furlang_Context context, Furlang_Module_Info info);

size_t furlang_module_add_function(Furlang_Context context, Furlang_Module module, Furlang_Module_Function function);
Furlang_Module_Function furlang_module_get_function(Furlang_Context context, Furlang_Module module, size_t index);

#endif // _FURLANG_MODULE_H_