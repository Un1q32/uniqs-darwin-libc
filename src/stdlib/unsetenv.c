#include <stdbool.h>
#include <stdlib.h>

extern char **environ;

int unsetenv(const char *name) {
  __environ_alloc();
  if (!__environ_allocated)
    return -1;
  int i = __findenv(name);
  if (i == -1)
    return 0;
  free(environ[i]);
  while (environ[i + 1] != NULL) {
    environ[i] = environ[i + 1];
    i++;
  }
  environ[i] = NULL;
  return 0;
}
