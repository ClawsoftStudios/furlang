#ifndef   _FURLANG_BYTECODE_H_
#define   _FURLANG_BYTECODE_H_

#include <stddef.h>
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

typedef struct Fbc_Header_Function {
  Furlang_Addr address;
  size_t paramCount;
} Fbc_Header_Function;

typedef struct Fbc_Header {
  uint8_t magic[4];
  uint32_t version;
  size_t entryFunction;
  size_t functionCount;
  Fbc_Header_Function *functions;
} Fbc_Header;

typedef struct Fbc {
  Fbc_Header header;
  size_t bytecodeLength;
  const char *bytecode;
} Fbc;

#define FBC_MAGIC { 'F', 'b', 'c', 0x69 }

#endif // _FURLANG_BYTECODE_H_