#include <sys/syscall.h>
#include <unistd.h>

int pipe(int pipefd[2]) {
  int pipefd1 = syscall(SYS_pipe);
  if (pipefd1 == -1)
    return -1;
  int pipefd2 = __syscallret2;
  pipefd[0] = pipefd1;
  pipefd[1] = pipefd2;
  return 0;
}
