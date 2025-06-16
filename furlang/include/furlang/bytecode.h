#ifndef   _FURLANG_BYTECODE_H_
#define   _FURLANG_BYTECODE_H_

typedef enum Furlang_Instruction {
  COUNT_FURLANG_INSTRUCTIONS
} Furlang_Instruction;

_Static_assert(COUNT_FURLANG_INSTRUCTIONS <= 0xFF, "Too many instructions!");

#endif // _FURLANG_BYTECODE_H_