#include "furlang/bytecode.h"

#include <stdio.h>
#include <assert.h>

static void write_module(Fbc_Module module, FILE *fd) {
  assert(fd);

  assert(fwrite(&module.functionCount, 8, 1, fd) == 1);
  for (size_t i = 0; i < module.functionCount; ++i) {
    Fbc_Module_Function func = module.functions[i];
    assert(fwrite(&func.address, sizeof(func.address), 1, fd) == 1);
    assert(fwrite(&func.paramCount, 2, 1, fd) == 1);
  }

  assert(fwrite(&module.globalVariableCount, 2, 1, fd) == 1);
  assert(fwrite(&module.bytecodeLength, 8, 1, fd) == 1);
  assert(fwrite(module.bytecode, module.bytecodeLength, 1, fd) == 1);
}

void furlang_write_fbc_to_file(Fbc fbc, const char *filepath) {
  assert(filepath);

  FILE *fd = fopen(filepath, "wb");
  assert(fd);

  assert(fwrite((uint8_t[])FBC_MAGIC, 4, 1, fd) == 1);
  assert(fwrite(&fbc.header.version, 4, 1, fd) == 1);
  assert(fwrite(&fbc.header.entryFunction, 16, 1, fd) == 1);
  assert(fwrite(&fbc.moduleCount, 8, 1, fd) == 1);
  for (size_t i = 0; i < fbc.moduleCount; ++i) write_module(fbc.modules[i], fd);

  fclose(fd);
}

static void read_module(Fbc_Module *module, FILE *fd, Furlang_Allocator *allocator) {
  assert(fread(&module->functionCount, 8, 1, fd) == 1);

  module->functions = furlang_allocator_alloc(allocator, sizeof(*module->functions) * module->functionCount);
  for (size_t i = 0; i < module->functionCount; ++i) {
    Fbc_Module_Function *func = &module->functions[i];
    assert(fread(&func->address, sizeof(func->address), 1, fd) == 1);
    assert(fread(&func->paramCount, 2, 1, fd) == 1);
  }

  assert(fread(&module->globalVariableCount, 2, 1, fd) == 1);
  assert(fread(&module->bytecodeLength, 8, 1, fd) == 1);

  char *bytecode = furlang_allocator_alloc(allocator, module->bytecodeLength);
  assert(bytecode);
  assert(fread(bytecode, module->bytecodeLength, 1, fd) == 1);
  module->bytecode = bytecode;
}

Fbc furlang_read_fbc_from_file(const char *filepath, Furlang_Allocator *allocator) {
  assert(filepath);

  Fbc fbc = {0};

  FILE *fd = fopen(filepath, "rb");
  assert(fd);

  assert(fread(&fbc.header.magic, 4, 1, fd) == 1);
  assert(fread(&fbc.header.version, 4, 1, fd) == 1);
  assert(fread(&fbc.header.entryFunction, 16, 1, fd) == 1);
  assert(fread(&fbc.moduleCount, 8, 1, fd) == 1);

  Fbc_Module *modules = furlang_allocator_alloc(allocator, sizeof(*fbc.modules) * fbc.moduleCount);
  assert(modules);
  for (size_t i = 0; i < fbc.moduleCount; ++i) read_module(&modules[i], fd, allocator);
  fbc.modules = modules;

  fclose(fd);

  return fbc;
}