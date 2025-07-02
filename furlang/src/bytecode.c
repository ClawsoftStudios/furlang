#include "furlang/bytecode.h"

#include <stdio.h>
#include <assert.h>

void furlang_write_fbc_to_file(Fbc fbc, const char *filepath) {
  assert(filepath);

  FILE *fd = fopen(filepath, "wb");
  assert(fd);

  assert(fwrite((uint8_t[])FBC_MAGIC, 4, 1, fd) == 1);
  assert(fwrite(&fbc.header.version, 4, 1, fd) == 1);
  assert(fwrite(&fbc.header.entryFunction, 8, 1, fd) == 1);
  assert(fwrite(&fbc.header.functionCount, 8, 1, fd) == 1);
  for (size_t i = 0; i < fbc.header.functionCount; ++i) {
    Fbc_Header_Function func = fbc.header.functions[i];
    assert(fwrite(&func.address, sizeof(func.address), 1, fd) == 1);
    assert(fwrite(&func.paramCount, 8, 1, fd) == 1);
  }
  assert(fwrite(&fbc.header.globalVariableCount, 2, 1, fd) == 1);
  assert(fwrite(&fbc.bytecodeLength, 8, 1, fd) == 1);
  assert(fwrite(fbc.bytecode, fbc.bytecodeLength, 1, fd) == 1);
}

Fbc furlang_read_fbc_from_file(const char *filepath, Furlang_Allocator *allocator) {
  assert(filepath);

  Fbc fbc = {0};

  FILE *fd = fopen(filepath, "wb");
  assert(fd);

  assert(fread((uint8_t[])FBC_MAGIC, 4, 1, fd) == 1);
  assert(fread(&fbc.header.version, 4, 1, fd) == 1);
  assert(fread(&fbc.header.entryFunction, 8, 1, fd) == 1);
  assert(fread(&fbc.header.functionCount, 8, 1, fd) == 1);
  for (size_t i = 0; i < fbc.header.functionCount; ++i) {
    Fbc_Header_Function func = fbc.header.functions[i];
    assert(fread(&func.address, sizeof(func.address), 1, fd) == 1);
    assert(fread(&func.paramCount, 8, 1, fd) == 1);
  }
  assert(fread(&fbc.header.globalVariableCount, 2, 1, fd) == 1);
  assert(fread(&fbc.bytecodeLength, 8, 1, fd) == 1);

  char *bytecode = furlang_allocator_alloc(allocator, fbc.bytecodeLength);
  assert(bytecode);
  assert(fread(bytecode, fbc.bytecodeLength, 1, fd) == 1);
  fbc.bytecode = bytecode;

  fclose(fd);

  return fbc;
}