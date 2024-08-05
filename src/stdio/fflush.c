#include <stdio.h>

int fflush(FILE *stream) {
  ssize_t writeret = stream->write(stream->fd, stream->buf, stream->bufcount);
  if (writeret == -1)
    return EOF;
  stream->bufcount = 0;
  return 0;
}
