#include <stdio.h>

#include <furlang/alloc/arena.h>
#include <furlang/alloc/allocator.h>

#include <furlang/utils/file.h>

#include "furas/lexer.h"
#include "furas/codegen.h"

int main(int argc, char **argv) {
  (void)argv;
  if (argc != 2) return 1;
  const char *const input = argv[1];

  Furlang_Arena arena = {0};
  Furlang_Allocator allocator = {
    .data = &arena,
    .alloc = (Furlang_Allocator_Alloc)furlang_arena_alloc
  };

  Furas_Lexer lexer = {
    .stringAllocator = &allocator,
    .content = furlang_read_file(input, &lexer.length, NULL)
  };

  furlang_write_fbc_to_file(furas_codegen(furas_lexer_tokenize(&lexer), NULL), "./a.fbc");

  return 0;
}