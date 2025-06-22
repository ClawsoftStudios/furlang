#ifndef   _FURC_UTILS_H_
#define   _FURC_UTILS_H_

#include <furlang/alloc/allocator.h>

size_t furc_load_file(const char *filepath, const char **content, Furlang_Allocator *allocator);

#endif // _FURC_UTILS_H_