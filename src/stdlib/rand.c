#include <stdlib.h>

int rand(void) {
  int ret = __rand(__random_seed);
  __random_seed = ret;
  return ret;
}
