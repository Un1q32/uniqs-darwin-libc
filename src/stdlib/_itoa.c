#include <stdlib.h>
#include <string.h>

char *_itoa(long long num) {
  if (num < 0) {
    char *ret = _utoa(-num);
    static char ret2[32];
    ret2[0] = '-';
    strcpy(ret2 + 1, ret);
    return ret2;
  } else
    return _utoa(num);
}
