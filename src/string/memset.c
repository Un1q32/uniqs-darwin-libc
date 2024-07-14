#include <stddef.h>

void *memset(void *buf, int byte, size_t n) {
  unsigned char *p = buf;
  while (n--)
    *p++ = (unsigned char)byte;
  return buf;
}
