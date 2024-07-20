#include <sys/syslimits.h>
#include <unistd.h>

char *getwd(char *buf) { return getcwd(buf, PATH_MAX); }
