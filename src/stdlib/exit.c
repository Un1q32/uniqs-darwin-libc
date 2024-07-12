#include <stdio.h>
#include <unistd.h>

void (*__atexit_funcs[32])(void);
void (**__atexit_funcs_extra)(void);
int __atexit_count = 0;

void exit(int status) {
  while (__atexit_count > 32)
    __atexit_funcs_extra[--__atexit_count - 32]();
  while (__atexit_count > 0)
    __atexit_funcs[--__atexit_count]();
  fcloseall();
  /* TODO: unlink all files created with tmpfile() */
  _exit(status);
  while (1)
    ;
}
