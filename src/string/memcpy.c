#include <stddef.h>

void *memcpy(void *dst, const void *src, size_t n) {
  char *d = dst;
  const char *s = src;
  while (n--)
    *d++ = *s++;
  return dst;
}
