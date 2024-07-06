#include <stdio.h>

#ifdef __MACH__
int main(int argc, const char *argv[], const char *envp[], const char *apple[]) {
#else
int main(int argc, const char *argv[], const char *envp[]) {
#endif
  printf("argc: %d\n", argc);
  int i;
  for (i = 0; i < argc; i++)
    printf("argv[%d]: %s\n", i, argv[i]);
  for (i = 0; envp[i] != NULL; i++)
    printf("envp[%d]: %s\n", i, envp[i]);
#ifdef __MACH__
  for (i = 0; apple[i] != NULL; i++)
    printf("apple[%d]: %s\n", i, apple[i]);
#endif
  return 0;
}
