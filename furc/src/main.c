#include "furc/furc.h"

#include <furlang/alloc/arena.h>

#include <stdio.h>
#include <assert.h>

int main(int argc, char **argv) {
  assert(argc == 2);

  // const char *const program = argv[0];
  const char *const input = argv[1];

  Furlang_Arena lexerArena = {0};
  Furlang_Arena parserArena = {0};

  Furlang_Allocator arenaAllocator = {
    .data = &lexerArena,
    .alloc = (Furlang_Allocator_Alloc)furlang_arena_alloc,
    .free = NULL,
    .realloc = NULL
  };

  Furc_Lexer lexer = { .stringAllocator = &arenaAllocator };
  lexer.length = furc_load_file(input, &lexer.content, &arenaAllocator);

  Furc_Tokens tokens = furc_lexer_tokenize(&lexer);

  arenaAllocator.data = &parserArena;

  Furc_Parser parser = {
    .allocator = &arenaAllocator,
    .tokenView = {
      .tokens = tokens.items,
      .length = tokens.count
    }
  };

  Furc_Program program = furc_parser_parse(&parser);
  (void)program;

  return 0;
}