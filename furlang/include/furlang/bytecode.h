#ifndef   _FURLANG_BYTECODE_H_
#define   _FURLANG_BYTECODE_H_

#include "furlang/alloc/allocator.h"

#include <stddef.h>
#include <stdint.h>

typedef uint64_t Furlang_Addr;

typedef enum Furlang_Instruction {
  FURLANG_INSTRUCTION_NOP = 0,

  FURLANG_INSTRUCTION_IPUSH,  // Push int
  FURLANG_INSTRUCTION_BIPUSH, // Push byte as int
  FURLANG_INSTRUCTION_FPUSH,  // Push function
  FURLANG_INSTRUCTION_SFPUSH, // Push short as function

  FURLANG_INSTRUCTION_DUP,
  FURLANG_INSTRUCTION_SWAP,
  FURLANG_INSTRUCTION_POP,
  FURLANG_INSTRUCTION_RET,

  FURLANG_INSTRUCTION_STR, // Store variable
  FURLANG_INSTRUCTION_LOD, // Load variable

  FURLANG_INSTRUCTION_STRG, // Store global variable
  FURLANG_INSTRUCTION_LODG, // Load global variable

  FURLANG_INSTRUCTION_JMP,  // Jump
  FURLANG_INSTRUCTION_JZ,   // Jump zero
  FURLANG_INSTRUCTION_JNZ,  // Jump not zero
  FURLANG_INSTRUCTION_CALL, // Call function

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

typedef struct Fbc_Module_Function {
  Furlang_Addr address;
  uint16_t paramCount;
} Fbc_Module_Function;

typedef struct Fbc_Module {
  size_t functionCount;
  Fbc_Module_Function *functions;
  uint16_t globalVariableCount;
  size_t bytecodeLength;
  const char *bytecode;
} Fbc_Module;

typedef struct Fbc_Header {
  uint8_t magic[4];
  uint32_t version;
  struct {
    uint64_t module;
    uint64_t function;
  } entryFunction;
} Fbc_Header;

typedef struct Fbc {
  Fbc_Header header;
  size_t moduleCount;
  const Fbc_Module *modules;
} Fbc;

#define FBC_MAGIC { 'F', 'b', 'c', 0x69 }

void furlang_write_fbc_to_file(Fbc fbc, const char *filepath);
Fbc furlang_read_fbc_from_file(const char *filepath, Furlang_Allocator *allocator);

#endif // _FURLANG_BYTECODE_H_