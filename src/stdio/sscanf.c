#include <stdio.h>

int sscanf(const char *str, const char *format, ...) {
  va_list ap;
  va_start(ap, format);
  int ret = vsscanf(str, format, ap);
  va_end(ap);
  return ret;
}
