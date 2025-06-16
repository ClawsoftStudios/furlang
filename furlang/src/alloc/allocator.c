#include "furlang/alloc/allocator.h"

#include <stdlib.h>
#include <assert.h>

void *furlang_allocator_alloc(Furlang_Allocator *allocator, size_t size) {
  if (!allocator || !allocator->alloc) return malloc(size);
  return allocator->alloc(allocator->data, size);
}

void furlang_allocator_free(Furlang_Allocator *allocator, void *ptr) {
  if (!allocator || (!allocator->free && !allocator->alloc)) free(ptr);
  else if (allocator->free) allocator->free(allocator->data, ptr);
}

void *furlang_allocator_realloc(Furlang_Allocator *allocator, void *ptr, size_t size) {
  if (!allocator || !allocator->realloc) {
    if (!allocator->alloc) return realloc(ptr, size);
    else {
      assert(0 && "Unimplemented");
    }
  }

  return allocator->realloc(allocator->data, ptr, size);
}