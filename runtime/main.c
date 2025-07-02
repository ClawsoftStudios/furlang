#include <furlang/furlang.h>
#include <furlang/bytecode.h>

#include <stdio.h>
#include <assert.h>

int main(int argc, char **argv) {
  if (argc == 1) {
    fprintf(stderr, "Usage: %s <input.fbc>\n", argv[0]);
    return 1;
  }

  Furlang_Context context = furlang_context_create(furlang_read_fbc_from_file(argv[1], NULL), NULL);
  furlang_context_run(context);
  furlang_context_destroy(context, NULL);

  return 0;
}