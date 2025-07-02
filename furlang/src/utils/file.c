#include "furlang/utils/file.h"

#include <stdio.h>
#include <assert.h>

char *furlang_read_file(const char *filepath, size_t *length, Furlang_Allocator *allocator) {
  assert(filepath);

  FILE *fd = fopen(filepath, "rb");
  if (!fd) return NULL;

  assert(fseek(fd, 0, SEEK_END) == 0);
  long size = ftell(fd);
  assert(size > 0);
  assert(fseek(fd, 0, SEEK_SET) == 0);

  char *content = furlang_allocator_alloc(allocator, size);
  if (!content) return content;

  assert(fread(content, size, 1, fd) == 1);
  fclose(fd);
  if (length) *length = size;

  return content;
}