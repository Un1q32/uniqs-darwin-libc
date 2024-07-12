#include <stdlib.h>

int putenv(char *string) { return __putenv(string, 1); }
