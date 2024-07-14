#include <stddef.h>

char *strncpy(char *dst, const char *src, size_t n) {
  char *ret = dst;
  while (n && (*dst++ = *src++))
    n--;
  while (n--)
    *dst++ = '\0';
  return ret;
}
