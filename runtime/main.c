#include <furlang/furlang.h>
#include <furlang/bytecode.h>

#include <assert.h>

int main(void) {
  char bytecode[] = {
         // main() - 0:
/*  0 */ FURLANG_INSTRUCTION_BIPUSH, 20,
/*  2 */ FURLANG_INSTRUCTION_SFPUSH, 0, 1,
/*  5 */ FURLANG_INSTRUCTION_CALL,
/*  6 */ FURLANG_INSTRUCTION_RET,
/*  7 */ // fib(n) - 1:
/*  7 */ FURLANG_INSTRUCTION_BIPUSH, 0,
/*  9 */ FURLANG_INSTRUCTION_STR, 0, 1,
/* 12 */ FURLANG_INSTRUCTION_BIPUSH, 1,
/* 14 */ FURLANG_INSTRUCTION_STR, 0, 2,
/* 17 */ // loop:
/* 17 */ FURLANG_INSTRUCTION_LOD, 0, 0,
/* 20 */ FURLANG_INSTRUCTION_JZ, FURLANG_BYTECODE_ADDR(54 /* end */),
/* 28 */ FURLANG_INSTRUCTION_LOD, 0, 1,
/* 31 */ FURLANG_INSTRUCTION_DUP,
/* 32 */ FURLANG_INSTRUCTION_LOD, 0, 2,
/* 35 */ FURLANG_INSTRUCTION_ADD,
/* 36 */ FURLANG_INSTRUCTION_LOD, 0, 2,
/* 39 */ FURLANG_INSTRUCTION_STR, 0, 1,
/* 42 */ FURLANG_INSTRUCTION_STR, 0, 2,
/* 45 */ FURLANG_INSTRUCTION_LOD, 0, 0,
/* 48 */ FURLANG_INSTRUCTION_BIPUSH, 1,
/* 50 */ FURLANG_INSTRUCTION_SUB,
/* 51 */ FURLANG_INSTRUCTION_STR, 0, 0,
/* 54 */ // end:
/* 54 */ FURLANG_INSTRUCTION_RET
  };

  Furlang_Context context = furlang_context_create((Fbc){
    .header = {
      .magic = FBC_MAGIC,
      .version = 0,
      .entryFunction = 0,
      .functionCount = 2,
      .functions = (Fbc_Header_Function[]){
        (Fbc_Header_Function){
          .address = 0,
          .paramCount = 0
        },
        (Fbc_Header_Function){
          .address = 7,
          .paramCount = 1
        }
      }
    },
    .bytecodeLength = sizeof(bytecode)/sizeof(*bytecode),
    .bytecode = bytecode
  }, NULL);

  furlang_context_run(context);

  furlang_context_destroy(context, NULL);

  return 0;
}