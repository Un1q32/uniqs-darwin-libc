#include <stddef.h>

void *memchr(const void *buf, int ch, size_t n) {
  const unsigned char *ptr = buf;
  while (n--) {
    if (*ptr == (unsigned char)ch)
      return (void *)ptr;
    ptr++;
  }
  return NULL;
}
