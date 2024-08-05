char *_utoX(unsigned long long num) {
  static char buf[32];
  char *p = buf + 31;
  *p = '\0';
  do {
    *--p = "0123456789ABCDEF"[num & 0xf];
    num >>= 4;
  } while (num);
  return p;
}
