#include <furlang/furlang.h>

#include <assert.h>

int main(void) {
  Furlang_Context context = furlang_context_create(NULL);

  Furlang_Executor executor = furlang_executor_create(context, 0);

  furlang_context_run(context);

  furlang_context_destroy(context, NULL);

  return 0;
}