#include <stdio.h>
#include <unistd.h>

int fflush(FILE *stream) {
  ssize_t writeret = write(stream->fd, stream->buf, stream->bufcount);
  if (writeret == -1)
    return EOF;
  stream->bufcount = 0;
  return 0;
}
