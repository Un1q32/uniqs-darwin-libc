#include <stdio.h>
#include <stdlib.h>

void fcloseall(void) {
  if (__open_stream_list) {
    size_t nstreams = *(size_t *)__open_stream_list;
    while (nstreams) {
      if (__open_stream_list[nstreams] != NULL)
        fclose(__open_stream_list[nstreams]);
      nstreams--;
    }
    free(__open_stream_list);
    __open_stream_list = NULL;
  }
  fclose(stdin);
  fclose(stdout);
  fclose(stderr);
}
