#include <furlang/furlang.h>
#include <furlang/bytecode.h>

#include <assert.h>

int main(void) {
  char bytecode[] = { // /furc/example/fib.fur
/*  0 */ FURLANG_INSTRUCTION_BIPUSH, 0,
/*  2 */ FURLANG_INSTRUCTION_STR, 0, 0,
/*  5 */ FURLANG_INSTRUCTION_BIPUSH, 1,
/*  7 */ FURLANG_INSTRUCTION_STR, 0, 1,
/*    */ // int i = 0
/* 10 */ FURLANG_INSTRUCTION_BIPUSH, 0,
/*    */ // i < 20
/* 12 */ FURLANG_INSTRUCTION_DUP,
/* 13 */ FURLANG_INSTRUCTION_BIPUSH, 20,
/* 15 */ FURLANG_INSTRUCTION_SUB, // TODO: Comparison instructions
/* 16 */ FURLANG_INSTRUCTION_JZ, FURLANG_BYTECODE_ADDR(57),
/* 25 */ FURLANG_INSTRUCTION_LOD, 0, 0,
/* 28 */ FURLANG_INSTRUCTION_SWAP,
/* 29 */ FURLANG_INSTRUCTION_LOD, 0, 0,
/* 32 */ FURLANG_INSTRUCTION_LOD, 0, 1,
/* 35 */ FURLANG_INSTRUCTION_ADD,
/* 36 */ FURLANG_INSTRUCTION_LOD, 0, 1,
/* 39 */ FURLANG_INSTRUCTION_STR, 0, 0,
/* 42 */ FURLANG_INSTRUCTION_STR, 0, 1,
/*    */ // ++i
/* 45 */ FURLANG_INSTRUCTION_BIPUSH, 1,
/* 47 */ FURLANG_INSTRUCTION_ADD,
/* 48 */ FURLANG_INSTRUCTION_JMP, FURLANG_BYTECODE_ADDR(12),
/* 57 */ FURLANG_INSTRUCTION_RET
  };
  size_t bytecodeLength = sizeof(bytecode)/sizeof(*bytecode);

  Furlang_Context context = furlang_context_create(bytecode, bytecodeLength, NULL);

  Furlang_Executor executor = furlang_executor_create(context, 0);

  furlang_context_run(context);

  furlang_context_destroy(context, NULL);

  return 0;
}