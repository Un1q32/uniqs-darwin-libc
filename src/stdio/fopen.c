#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

FILE *fopen(const char *filename, const char *mode) {
  int flags = __fopen_mode_parse(mode);
  if (flags == -1) {
    errno = EINVAL;
    return NULL;
  }
  int fd = open(filename, flags, 0666);
  if (fd < 0)
    return NULL;
  FILE *fp = __fdopen(fd, flags);
  if (fp == NULL)
    close(fd);
  return fp;
}
