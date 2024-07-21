#include <errno.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <unistd.h>

int chmod(const char *path, mode_t mode) {
  int ret = syscall(SYS_chmod, path, mode);
  if (ret == -1 || errno != EPERM || (mode & (S_ISUID | S_ISGID)) == 0)
    return ret;
  if (mode & S_ISGID) {
    ret = syscall(SYS_chmod, path, mode ^ S_ISGID);
    if (ret == -1 || errno != EPERM)
      return ret;
  }
  if (mode & S_ISUID) {
    ret = syscall(SYS_chmod, path, mode ^ S_ISUID);
    if (ret == -1 || errno != EPERM)
      return ret;
  }
  if ((mode & (S_ISUID | S_ISGID)) == (S_ISUID | S_ISGID))
    ret = syscall(SYS_chmod, path, mode ^ (S_ISUID | S_ISGID));
  return ret;
}
