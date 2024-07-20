#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

FILE *__fdopen(int fd, mode_t flags) {
  FILE *ret = malloc(sizeof(FILE));
  if (ret == NULL)
    return NULL;
  ret->buf = malloc(BUFSIZ);
  if (ret->buf == NULL) {
    free(ret);
    return NULL;
  }

  ret->fd = fd;
  ret->flags = 0;
  ret->bufcount = 0;
  ret->read = read;
  ret->write = write;

  if (!__open_stream_list) {
    __open_stream_list = malloc(sizeof(FILE *) * 2);
    if (!__open_stream_list) {
      free(ret->buf);
      free(ret);
      return NULL;
    }

    *((size_t *)__open_stream_list) = 1;
    __open_stream_list[1] = ret;
    ret->listpos = 1;
  } else {
    FILE **new_list =
        realloc(__open_stream_list,
                sizeof(FILE *) * (*((size_t *)__open_stream_list) + 2));
    if (!new_list) {
      free(ret->buf);
      free(ret);
      return NULL;
    }
    __open_stream_list = new_list;
    size_t *nstreams = (size_t *)__open_stream_list;
    __open_stream_list[++*nstreams] = ret;
    ret->listpos = *nstreams;
  }

  if (flags & O_CLOEXEC) {
    fcntl(fd, F_SETFD, FD_CLOEXEC);
    flags &= ~O_CLOEXEC;
  }

  if (isatty(fd))
    ret->flags |= __SLBF;

  return ret;
}
