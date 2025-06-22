#ifndef   _FURLANG_UTILS_H_
#define   _FURLANG_UTILS_H_

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

#endif // _FURLANG_UTILS_H_