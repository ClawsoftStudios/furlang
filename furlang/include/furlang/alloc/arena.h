#ifndef   _FURLANG_ALLOC_ARENA_H_
#define   _FURLANG_ALLOC_ARENA_H_

#include <stddef.h>
#include <stdint.h>

typedef struct Furlang_Region *Furlang_Region;
struct Furlang_Region {
  Furlang_Region next;

  size_t capacity;
  size_t count;
  uintptr_t data[];
};

Furlang_Region furlang_region_create(size_t capacity);
void furlang_region_destroy(Furlang_Region region);

typedef struct Furlang_Arena {
  Furlang_Region head, tail;
} Furlang_Arena;

Furlang_Arena *furlang_arena_create();
void furlang_arena_destroy(Furlang_Arena *arena);

void *furlang_arena_alloc(Furlang_Arena *arena, size_t sizeInBytes);
void furlang_arena_reset(Furlang_Arena *arena);
void furlang_arena_free(Furlang_Arena *arena);

#endif // _FURLANG_ALLOC_ARENA_H_