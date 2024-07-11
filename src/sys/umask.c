#include <sys/syscall.h>
#include <unistd.h>

int umask(mode_t mask) { return syscall(SYS_umask, mask); }
