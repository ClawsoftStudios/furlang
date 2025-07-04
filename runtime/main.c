#include <furlang/furlang.h>
#include <furlang/bytecode.h>

#include <stdio.h>
#include <assert.h>

int main(int argc, char **argv) {
  if (argc == 1) {
    fprintf(stderr, "Usage: %s <input.fbc>\n", argv[0]);
    return 1;
  }

  Fbc fbc = furlang_read_fbc_from_file(argv[1], NULL);

  Furlang_Context context = furlang_context_create(NULL);

  for (size_t i = 0; i < fbc.moduleCount; ++i) {
    Fbc_Module fbcModule = fbc.modules[i];

    Furlang_Module_Function functions[fbcModule.functionCount] = {};
    for (size_t j = 0; j < fbcModule.functionCount; ++j) {
      functions[j] = (Furlang_Module_Function){
        .address = fbcModule.functions[j].address,
        .paramCount = fbcModule.functions[j].paramCount
      };
    }

    (void)furlang_module_create(context, (Furlang_Module_Info){
      .functionCount = fbcModule.functionCount,
      .functions = functions,
      .globalVariableCount = fbcModule.globalVariableCount,
      .bytecodeLength = fbcModule.bytecodeLength,
      .bytecode = fbcModule.bytecode
    });
  }

  furlang_executor_create(context, fbc.header.entryFunction.module, fbc.header.entryFunction.function);

  furlang_context_run(context);
  furlang_context_destroy(context, NULL);

  return 0;
}