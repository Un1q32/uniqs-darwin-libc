#include <stdio.h>

void fcloseall(void) {
  size_t nstreams = *(size_t *)__open_stream_list;
  while (nstreams) {
    if (__open_stream_list[nstreams] != NULL)
      fclose(__open_stream_list[nstreams]);
    nstreams--;
  }
  *__open_stream_list = NULL;
  fclose(stdin);
  fclose(stdout);
  fclose(stderr);
}
