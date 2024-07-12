#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

extern char **environ;

void __environ_alloc(void) {
  if (__environ_allocated)
    return;
  int environ_len = 0;
  while (environ[environ_len] != NULL)
    environ_len++;
  char **new_environ = malloc((environ_len + 1) * sizeof(char *));
  if (new_environ == NULL)
    return;
  for (int i = 0; i < environ_len; i++) {
    new_environ[i] = strdup(environ[i]);
    if (new_environ[i] == NULL) {
      for (int j = 0; j < i; j++)
        free(new_environ[j]);
      free(new_environ);
      return;
    }
  }
  new_environ[environ_len] = NULL;
  environ = new_environ;
  __environ_allocated = true;
}
