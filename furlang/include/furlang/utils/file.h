#ifndef   _FURLANG_UTILS_FILE_H_
#define   _FURLANG_UTILS_FILE_H_

#include "furlang/alloc/allocator.h"

char *furlang_read_file(const char *filepath, size_t *length, Furlang_Allocator *allocator);

#endif // _FURLANG_UTILS_FILE_H_