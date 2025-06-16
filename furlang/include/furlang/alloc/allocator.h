#ifndef   _FURLANG_ALLOC_ALLOCATOR_H_
#define   _FURLANG_ALLOC_ALLOCATOR_H_

#include <stddef.h>

typedef void *(*Furlang_Allocator_Alloc)(void *, size_t);
typedef void  (*Furlang_Allocator_Free)(void *, void *);
typedef void *(*Furlang_Allocator_Realloc)(void *, void *, size_t);

typedef struct Furlang_Allocator {
  void *data;
  Furlang_Allocator_Alloc alloc;
  Furlang_Allocator_Free free;
  Furlang_Allocator_Realloc realloc;
} Furlang_Allocator;

void *furlang_allocator_alloc(Furlang_Allocator *allocator, size_t size);
void  furlang_allocator_free(Furlang_Allocator *allocator, void *ptr);
void *furlang_allocator_realloc(Furlang_Allocator *allocator, void *ptr, size_t size);

#endif // _FURLANG_ALLOC_ALLOCATOR_H_