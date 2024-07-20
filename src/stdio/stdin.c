#include <stdio.h>
#include <unistd.h>

static char stdin_buf[BUFSIZ];

FILE _stdin = {STDIN_FILENO, __SRD, stdin_buf, 0, 0, read, write};

FILE *stdin = &_stdin;
