#ifndef   _FURLANG_UTILS_SPARSE_SET_H_
#define   _FURLANG_UTILS_SPARSE_SET_H_

#include "furlang/utils/da.h"

#define FURLANG_SPARSE_SET(name, type, sparseType, pageSize) \
  typedef struct name { \
    struct { \
      sparseType (*items)[pageSize]; \
      size_t capacity; \
      size_t count; \
    } sparse; \
    struct { \
      type *items; \
      size_t capacity; \
      size_t count; \
    } dense; \
    struct { \
      sparseType *items; \
      size_t capacity; \
      size_t count; \
    } denseToSparse; \
  } name

#define FURLANG_SPARSE_SET_PAGE_SIZE(set) sizeof(*(set)->sparse.items)/sizeof(**(set)->sparse.items)

#define FURLANG_SPARSE_SET_SET_DENSE_INDEX(set, id, idx)                                                  \
  do {                                                                                                    \
    size_t page = id/FURLANG_SPARSE_SET_PAGE_SIZE(set);                                                   \
    if (page >= (set)->sparse.count) {                                                                    \
      size_t oldCount = (set)->sparse.count;                                                              \
      FURLANG_DA_RESIZE(&(set)->sparse, page+1);                                                          \
                                                                                                          \
      for (size_t i = oldCount; i <= page; ++i)                                                           \
        memset((set)->sparse.items[i], 0xFF, sizeof((set)->sparse.items[i]));                             \
    }                                                                                                     \
                                                                                                          \
    (set)->sparse.items[page][id%FURLANG_SPARSE_SET_PAGE_SIZE(set)] = idx;                                \
  } while (0)

#define FURLANG_SPARSE_SET_GET_DENSE_INDEX(set, sparseType, id)                                          \
    ((id/FURLANG_SPARSE_SET_PAGE_SIZE(set) >= (set)->sparse.count)                                       \
      ? ((sparseType)-1)                                                                                 \
      : ((set)->sparse.items[id/FURLANG_SPARSE_SET_PAGE_SIZE(set)][id%FURLANG_SPARSE_SET_PAGE_SIZE(set)]))

#define FURLANG_SPARSE_SET_PUT(set, sparseType, id, value)                    \
  do {                                                                        \
    sparseType idx = FURLANG_SPARSE_SET_GET_DENSE_INDEX(set, sparseType, id); \
    if (idx != (sparseType)-1) {                                              \
      FURLANG_DA_AT(&(set)->dense, idx) = value;                              \
      FURLANG_DA_AT(&(set)->denseToSparse, idx) = id;                         \
      break;                                                                  \
    }                                                                         \
                                                                              \
    FURLANG_SPARSE_SET_SET_DENSE_INDEX(set, id, (set)->dense.count);          \
    FURLANG_DA_APPEND(&(set)->dense, value);                                  \
    FURLANG_DA_APPEND(&(set)->denseToSparse, id);                             \
  } while (0)

#define FURLANG_SPARSE_SET_REMOVE(set, sparseType, id)                                                      \
  do {                                                                                                      \
    sparseType idx = FURLANG_SPARSE_SET_GET_DENSE_INDEX(set, sparseType, id);                               \
    if (idx == (sparseType)-1) break;                                                                       \
                                                                                                            \
    FURLANG_SPARSE_SET_SET_DENSE_INDEX(set, (set)->denseToSparse.items[(set)->denseToSparse.count-1], idx); \
    FURLANG_SPARSE_SET_SET_DENSE_INDEX(set, id, (sparseType)-1);                                            \
                                                                                                            \
    (set)->dense.items[idx] = (set)->dense.items[--(set)->dense.count];                                     \
    (set)->denseToSparse.items[idx] = (set)->denseToSparse.items[--(set)->denseToSparse.count];             \
  } while (0)

#define FURLANG_SPARSE_SET_GET(set, sparseType, id) (assert(FURLANG_SPARSE_SET_GET_DENSE_INDEX(set, sparseType, id) != (sparseType)-1), \
  &(set)->dense.items[FURLANG_SPARSE_SET_GET_DENSE_INDEX(set, sparseType, id)])

#define FURLANG_SPARSE_SET_FOREACH(set, sparseType, it) \
  for (sparseType *it = (set)->denseToSparse.items; it != &(set)->denseToSparse.items[(set)->denseToSparse.count]; ++it)

#define FURLANG_SPARSE_SET_SIZE(set) (set)->denseToSparse.count

#endif // _FURLANG_UTILS_SPARSE_SET_H_