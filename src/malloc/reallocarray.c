#include <malloc.h>

void *reallocarray(void *ptr, size_t nmemb, size_t size) {
  if (nmemb > 0 && size > 0) {
    size_t total_size = nmemb * size;
    if (total_size / nmemb == size)
      return realloc(ptr, total_size);
  }
  return NULL;
}
