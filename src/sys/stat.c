#include <sys/stat.h>
#include <sys/syscall.h>
#include <unistd.h>

int stat(const char *path, struct stat *buf) {
  return syscall(SYS_stat64, path, buf);
}
