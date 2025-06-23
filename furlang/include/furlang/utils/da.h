#ifndef   _FURLANG_UTILS_H_
#define   _FURLANG_UTILS_H_

#include <stdlib.h>
#include <assert.h>

#define max(a, b) ((a)>(b))?(a):(b)

#define FURLANG_DA_APPEND(da, value)                                             \
  do {                                                                           \
    if ((da)->count >= (da)->capacity) {                                         \
      if ((da)->capacity) (da)->capacity <<= 1;                                  \
      else (da)->capacity = 16;                                                  \
      (da)->items = realloc((da)->items, sizeof(*(da)->items) * (da)->capacity); \
      assert((da)->items);                                                       \
    }                                                                            \
                                                                                 \
    (da)->items[(da)->count++] = value;                                          \
  } while (0)

#define FURLANG_DA_APPEND_MANY(da, values, count)                                \
  do {                                                                           \
    if ((da)->count+count > (da)->capacity) {                                    \
      if (!(da)->capacity) (da)->capacity = 16;                                  \
      while ((da)->count+count >= (da)->capacity) (da)->capacity <<= 1;          \
      (da)->items = realloc((da)->items, sizeof(*(da)->items) * (da)->capacity); \
      assert((da)->items);                                                       \
    }                                                                            \
                                                                                 \
    memcpy(&(da)->items[(da)->count], values, sizeof(*(da)->items) * count);     \
    (da)->count += count;                                                        \
  } while (0)

#define FURLANG_DA_FAST_REMOVE(da, idx)            \
  do {                                             \
    assert(idx < (da)->count);                     \
    (da)->items[idx] = (da)->items[--(da)->count]; \
  } while (0)

#define FURLANG_DA_POP_BACK(da) (assert((da)->count), (da)->items[--(da)->count])

#define FURLANG_DA_RESIZE(da, newSize)                                         \
  do {                                                                         \
    (da)->count = max((da)->count, newSize);                                   \
                                                                               \
    if (!(da)->capacity) (da)->capacity = 16;                                  \
    while ((da)->count >= (da)->capacity) (da)->capacity <<= 1;                \
    (da)->items = realloc((da)->items, sizeof(*(da)->items) * (da)->capacity); \
    assert((da)->items);                                                       \
  } while (0)

#define FURLANG_DA_AT(da, idx) assert(idx < (da)->count), (da)->items[idx]

#endif // _FURLANG_UTILS_H_