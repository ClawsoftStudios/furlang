#ifndef   _FURLANG_BYTECODE_H_
#define   _FURLANG_BYTECODE_H_

#include <stdint.h>

typedef uint64_t Furlang_Addr;

typedef enum Furlang_Instruction {
  FURLANG_INSTRUCTION_NOP = 0,

  FURLANG_INSTRUCTION_IPUSH,  // Push int
  FURLANG_INSTRUCTION_BIPUSH, // Push byte as int

  FURLANG_INSTRUCTION_DUP,
  FURLANG_INSTRUCTION_SWAP,
  FURLANG_INSTRUCTION_POP,
  FURLANG_INSTRUCTION_RET,

  FURLANG_INSTRUCTION_STR, // Store variable
  FURLANG_INSTRUCTION_LOD, // Load variable

  FURLANG_INSTRUCTION_JMP, // Jump
  FURLANG_INSTRUCTION_JZ,  // Jump zero
  FURLANG_INSTRUCTION_JNZ, // Jump not zero

  FURLANG_INSTRUCTION_ADD,
  FURLANG_INSTRUCTION_SUB,
  FURLANG_INSTRUCTION_MUL,
  FURLANG_INSTRUCTION_DIV,
  FURLANG_INSTRUCTION_MOD,

  COUNT_FURLANG_INSTRUCTIONS
} Furlang_Instruction;

_Static_assert(COUNT_FURLANG_INSTRUCTIONS <= 0xFF, "Too many instructions!");

#define FURLANG_BYTECODE_ADDR(addr) \
  (((Furlang_Addr)addr)>>(8*7)&0xFF), (((Furlang_Addr)addr)>>(8*6)&0xFF), (((Furlang_Addr)addr)>>(8*5)&0xFF), (((Furlang_Addr)addr)>>(8*4)&0xFF), \
  (((Furlang_Addr)addr)>>(8*3)&0xFF), (((Furlang_Addr)addr)>>(8*2)&0xFF), (((Furlang_Addr)addr)>>(8*1)&0xFF), (((Furlang_Addr)addr)>>(8*0)&0xFF)

#define FURLANG_BYTECODE_MAGIC ((uint8_t[4]){ 'F', 'b', 'c', \069 })

#endif // _FURLANG_BYTECODE_H_