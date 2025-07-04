#ifndef   __FURLANG_CONTEXT_H_
#define   __FURLANG_CONTEXT_H_

#include "furlang/context.h"

#include "./executor.h"
#include "./module.h"
#include "./thing.h"

#include "furlang/alloc/arena.h"

#include "furlang/utils/sparseSet.h"

FURLANG_SPARSE_SET(_Furlang_Executor_Sparse_Set, _Furlang_Executor, Furlang_Executor, 4);
FURLANG_SPARSE_SET(_Furlang_Thing_Sparse_Set, _Furlang_Thing, Furlang_Thing, 8);

typedef struct _Furlang_Dead_Thing {
  Furlang_Thing_Type type;
  Furlang_Thing id;
  char *data;
} _Furlang_Dead_Thing;

struct _Furlang_Context {
  struct {
    _Furlang_Module *items;
    size_t capacity;
    size_t count;
  } modules;

  _Furlang_Executor_Sparse_Set executors;
  struct {
    Furlang_Executor *items;
    size_t capacity;
    size_t count;
  } deadExecutors; // FILO

  _Furlang_Thing_Sparse_Set things;
  struct {
    _Furlang_Dead_Thing *items;
    size_t capacity;
    size_t count;
  } deadThings;

  Furlang_Arena thingsDataArena; // Arena for things' data
};

Furlang_Executor _furlang_context_append_executor(Furlang_Context context, Furlang_Position position);
_Furlang_Executor *_furlang_context_get_executor(Furlang_Context context, Furlang_Executor id);
void _furlang_context_remove_executor(Furlang_Context context, Furlang_Executor id);

Furlang_Thing _furlang_context_append_thing(Furlang_Context context, Furlang_Thing_Type type);
_Furlang_Thing *_furlang_context_get_thing(Furlang_Context context, Furlang_Thing id);
void _furlang_context_remove_thing(Furlang_Context context, Furlang_Thing id);

#endif // __FURLANG_CONTEXT_H_