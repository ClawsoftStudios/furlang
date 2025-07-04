#ifndef   __FURLANG_MODULE_H_
#define   __FURLANG_MODULE_H_

#include "furlang/module.h"

#include "furlang/bytecode.h"

#include "./thing.h"

typedef struct _Furlang_Module {
  struct {
    Furlang_Thing *items;
    size_t count;
  } globalVariables;

  struct {
    Furlang_Module_Function *items;
    size_t capacity;
    size_t count;
  } functions;

  size_t bytecodeLength;
  char *bytecode;
} _Furlang_Module;

void _furlang_module_store_global_var(Furlang_Context context, _Furlang_Module *module, uint16_t index, Furlang_Thing thing);
Furlang_Thing _furlang_module_load_global_var(Furlang_Context context, _Furlang_Module *module, uint16_t index);

#endif // __FURLANG_MODULE_H_