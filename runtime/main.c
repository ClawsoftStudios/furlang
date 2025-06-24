#include <furlang/furlang.h>
#include <furlang/bytecode.h>

#include <assert.h>

int main(void) {
  char bytecode[] = {
    FURLANG_INSTRUCTION_BIPUSH, 34,
    FURLANG_INSTRUCTION_BIPUSH, 35,
    FURLANG_INSTRUCTION_ADD,
    FURLANG_INSTRUCTION_RET
  };
  size_t bytecodeLength = sizeof(bytecode)/sizeof(*bytecode);

  Furlang_Context context = furlang_context_create(bytecode, bytecodeLength, NULL);

  Furlang_Executor executor = furlang_executor_create(context, 0);

  furlang_context_run(context);

  furlang_context_destroy(context, NULL);

  return 0;
}