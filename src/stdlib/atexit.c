#include <stdlib.h>

int atexit(void (*func)(void)) {
  if (__atexit_count < 32)
    __atexit_funcs[__atexit_count++] = func;
  else {
    void (**tmp)(void) =
        realloc(__atexit_funcs_extra, (__atexit_count - 32 + 1) * sizeof(void *));
    if (tmp == NULL)
      return -1;
    __atexit_funcs_extra = tmp;
    __atexit_funcs_extra[__atexit_count++ - 32] = func;
  }
  return 0;
}
