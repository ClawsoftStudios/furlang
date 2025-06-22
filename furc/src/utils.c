#include "furc/utils.h"

#include <stdio.h>
#include <assert.h>

size_t furc_load_file(const char *filepath, const char **content, Furlang_Allocator *allocator) {
  assert(filepath && content);

  FILE *fd = fopen(filepath, "rb");
  assert(fd);

  assert(!fseek(fd, 0, SEEK_END));

  long length = ftell(fd);
  assert(length > 0);

  assert(!fseek(fd, 0, SEEK_SET));

  char *c = furlang_allocator_alloc(allocator, length);
  assert(c);

  assert(fread(c, length, 1, fd));

  *content = c;

  fclose(fd);

  return (size_t)length;
}