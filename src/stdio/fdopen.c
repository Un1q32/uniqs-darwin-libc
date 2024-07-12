#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

FILE *fdopen(int fd, const char *mode) {
  int flags = __fopen_mode_parse(mode);
  if (flags == -1) {
    errno = EINVAL;
    return NULL;
  }
  return __fdopen(fd, flags);
}
