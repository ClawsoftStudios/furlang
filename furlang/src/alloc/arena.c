#include "furlang/alloc/arena.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define max(a, b) ((a)>(b))?(a):(b)

#define MIN_REGION_CAPACITY 4096

Furlang_Region furlang_region_create(size_t capacity) {
  Furlang_Region region = malloc(sizeof(*region) + sizeof(uintptr_t) * capacity);
  assert(region);
  region->next = NULL;
  region->capacity = capacity;
  region->count = 0;

  return region;
}

void furlang_region_destroy(Furlang_Region region) {
  free(region);
}



Furlang_Arena *furlang_arena_create() {
  Furlang_Arena *arena = malloc(sizeof(*arena));
  assert(arena);
  memset(arena, 0, sizeof(*arena));

  return arena;
}

void furlang_arena_destroy(Furlang_Arena *arena) {
  furlang_arena_free(arena);
  free(arena);
}

void *furlang_arena_alloc(Furlang_Arena *arena, size_t sizeInBytes) {
  assert(arena);

  size_t size = (sizeInBytes+sizeof(uintptr_t)-1) / sizeof(uintptr_t);

  if (!arena->head) {
    assert(!arena->tail);

    arena->head = arena->tail = furlang_region_create(max(size, MIN_REGION_CAPACITY));
  }

  Furlang_Region region = arena->head;
  while (region && region->capacity < region->count+size) region = region->next;
  if (!region) {
    region = furlang_region_create(max(size, MIN_REGION_CAPACITY));
    arena->tail = arena->tail->next = region;
  }


  uintptr_t *ptr = &region->data[region->count];
  region->count += size;
  return (void*)ptr;
}

void furlang_arena_reset(Furlang_Arena *arena) {
  assert(arena);

  for (Furlang_Region region = arena->head; region; region = region->next) {
    region->count = 0;
  }
}

void furlang_arena_free(Furlang_Arena *arena) {
  assert(arena);

  for (Furlang_Region region = arena->head, next = NULL; region; region = next) {
    next = region->next;
    furlang_region_destroy(region);
  }
}