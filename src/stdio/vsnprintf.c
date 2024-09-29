#include <errno.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

static char *__utoa(unsigned long long num, char *buf, unsigned char base,
                    bool upper) {
  char *chars;
  if (upper)
    chars = "0123456789ABCDEF";
  else
    chars = "0123456789abcdef";
  char *p = buf + 22;
  *p = '\0';
  do {
    *--p = chars[num % base];
    num /= base;
  } while (num);
  return p;
}

static char *__ftoa(long double num, unsigned int precision, char *buf) {
  if (num != num) {
    strcpy(buf, "nan");
    return buf;
  }
  buf[0] = '0';
  char *p = buf + 1;
  if (num < 0) {
    *p++ = '-';
    num = -num;
  }
  unsigned long long integer = num;
  num -= integer;
  char intbuf[21];
  char *intstr = __utoa(integer, intbuf, 10, true);
  while (*intstr)
    *p++ = *intstr++;
  if (precision) {
    *p++ = '.';
    while (precision--) {
      num *= 10;
      *p++ = '0' + (int)num;
      num -= (int)num;
    }
  }
  num *= 10;
  if ((int)num >= 5) {
    char *q = p - 1;
    while (*q == '9' || *q == '.') {
      if (*q == '.')
        --q;
      else
        *q-- = '0';
    }
    if (q == buf)
      *q = '1';
    else
      ++*q;
  }
  *p = '\0';
  if (buf[0] == '0')
    return buf + 1;
  return buf;
}

int vsnprintf(char *str, size_t size, const char *format, va_list ap) {
  size_t i = 0, j = 0;
  if (!str)
    size = 0;
  while (format[i]) {
    if (format[i] == '%') {
      ssize_t argstrlen = 0;
      const char *fmt = format + i + 1, *fmt2 = format + i;
      bool done = false, altform = false, precisionset = false,
           negative = false;
      uint8_t flags = 0;
      int fill = 0, precision = 6;
      char fillchar = ' ', sign = '\0';
      while (!done) {
        if (*fmt == '#')
          altform = true;
        else if (*fmt == '.') {
          if (fmt[1] == '*') {
            precision = va_arg(ap, int);
            if (precision < 0)
              precision = 6;
            fmt++;
          } else {
            precision = 0;
            while (fmt[1] >= '0' && fmt[1] <= '9') {
              precision *= 10;
              precision += fmt[1] - '0';
              fmt++;
            }
          }
          precisionset = true;
        } else if (*fmt == '0')
          fillchar = '0';
        else if (*fmt >= '1' && *fmt <= '9') {
          fill = 0;
          do {
            fill *= 10;
            fill += *fmt - '0';
            fmt++;
          } while (*fmt >= '0' && *fmt <= '9');
          fmt--;
          if (negative)
            fill = -fill;
        } else if (*fmt == '-')
          negative = true;
        else if (*fmt == '*') {
          fill = va_arg(ap, int);
          if (negative)
            fill = -fill;
        } else if (*fmt == ' ') {
          if (!sign)
            sign = ' ';
        } else if (*fmt == '+')
          sign = '+';
        else {
          fmt--;
          done = true;
        }
        fmt++;
      }
      done = false;
      while (!done) {
        switch (*fmt++) {
        case 'l':
          if (flags & 1 << 0)
            flags |= 1 << 1;
          else
            flags |= 1 << 0;
          break;
        case 'h':
          if (flags & 1 << 2)
            flags |= 1 << 3;
          else
            flags |= 1 << 2;
          break;
        case 'j':
          flags |= 1 << 4;
          break;
        case 't':
          flags |= 1 << 5;
          break;
        case 'z':
          flags |= 1 << 6;
          break;
        case 'L':
          flags |= 1 << 7;
          break;
        case 'q':
          flags |= (1 << 0) | (1 << 1);
          break;
        default:
          fmt--;
          done = true;
          break;
        }
      }
      char *tmp = NULL;
      size_t k = 0;
      char utoabuf[23];
      switch (*fmt) {
      case '%':
        if (j < size)
          str[j] = '%';
        j++;
        break;
      case 'S':
        flags |= 1 << 0;
        /* fall through */
      case 's':
        if (flags & 1 << 0) {
          wchar_t *tmp2 = va_arg(ap, wchar_t *);
          if (tmp2 == NULL)
            tmp2 = L"(null)";
          if (!precisionset)
            while (tmp2[argstrlen])
              argstrlen++;
          else
            while (tmp2[argstrlen] && argstrlen < precision)
              argstrlen++;
          precision = argstrlen;
          while (fill > argstrlen) {
            if (j < size)
              str[j] = fillchar;
            j++;
            fill--;
          }
          while (precision--) {
            if (j < size)
              str[j] = tmp2[k];
            j++;
            k++;
          }
          fill += argstrlen;
          while (fill < 0) {
            if (j < size)
              str[j] = ' ';
            j++;
            fill++;
          }
        } else {
          tmp = va_arg(ap, char *);
          if (tmp == NULL)
            tmp = "(null)";
          if (!precisionset)
            while (tmp[argstrlen])
              argstrlen++;
          else
            while (tmp[argstrlen] && argstrlen < precision)
              argstrlen++;
          precision = argstrlen;
          while (fill > argstrlen) {
            if (j < size)
              str[j] = fillchar;
            j++;
            fill--;
          }
          while (precision--) {
            if (j < size)
              str[j] = tmp[k];
            j++;
            k++;
          }
          fill += argstrlen;
          while (fill < 0) {
            if (j < size)
              str[j] = ' ';
            j++;
            fill++;
          }
        }
        break;
      case 'C':
        flags |= 1 << 0;
        /* fall through */
      case 'c':
        while (fill > 1) {
          if (j < size)
            str[j] = fillchar;
          j++;
          fill--;
        }
        if (j < size)
          str[j] = va_arg(ap, int);
        j++;
        fill++;
        while (fill < 0) {
          if (j < size)
            str[j] = ' ';
          j++;
          fill++;
        }
        break;
      case 'D':
        flags |= 1 << 0;
        /* fall through */
      case 'd':
      case 'i':
        if (flags & 1 << 4) {
          intmax_t num = va_arg(ap, intmax_t);
          if (num < 0) {
            sign = '-';
            num = -num;
          }
          tmp = __utoa(num, utoabuf, 10, false);
        } else if (flags & 1 << 1) {
          long long num = va_arg(ap, long long);
          if (num < 0) {
            sign = '-';
            num = -num;
          }
          tmp = __utoa(num, utoabuf, 10, false);
        } else if (flags & 1 << 5 || flags & 1 << 6) {
          ptrdiff_t num = va_arg(ap, ptrdiff_t);
          if (num < 0) {
            sign = '-';
            num = -num;
          }
          tmp = __utoa(num, utoabuf, 10, false);
        } else if (flags & 1 << 0) {
          long num = va_arg(ap, long);
          if (num < 0) {
            sign = '-';
            num = -num;
          }
          tmp = __utoa(num, utoabuf, 10, false);
        } else if (flags & 1 << 2) {
          short num = va_arg(ap, int);
          if (num < 0) {
            sign = '-';
            num = -num;
          }
          tmp = __utoa(num, utoabuf, 10, false);
        } else if (flags & 1 << 3) {
          char num = va_arg(ap, int);
          if (num < 0) {
            sign = '-';
            num = -num;
          }
          tmp = __utoa(num, utoabuf, 10, false);
        } else {
          int num = va_arg(ap, int);
          if (num < 0) {
            sign = '-';
            num = -num;
          }
          tmp = __utoa(num, utoabuf, 10, false);
        }
        argstrlen = strlen(tmp);
        if (!precisionset)
          precision = 1;
        else
          fillchar = ' ';
        if (!precision && tmp[0] == '0') {
          tmp = "";
          argstrlen = 0;
        }
        precision -= argstrlen;
        if (precision < 0)
          precision = 0;
        if (sign) {
          argstrlen++;
          if (fillchar == '0') {
            if (j < size)
              str[j] = sign;
            j++;
            sign = 0;
          }
        }
        while (fill > argstrlen + precision) {
          if (j < size)
            str[j] = fillchar;
          j++;
          fill--;
        }
        if (sign) {
          if (j < size)
            str[j] = sign;
          j++;
        }
        argstrlen += precision;
        while (precision--) {
          if (j < size)
            str[j] = '0';
          j++;
        }
        while (tmp[k]) {
          if (j < size)
            str[j] = tmp[k];
          j++;
          k++;
        }
        fill += argstrlen;
        while (fill < 0) {
          if (j < size)
            str[j] = ' ';
          j++;
          fill++;
        }
        break;
      case 'U':
        flags |= 1 << 0;
        /* fall through */
      case 'u':
        if (flags & 1 << 4)
          tmp = __utoa(va_arg(ap, uintmax_t), utoabuf, 10, false);
        else if (flags & 1 << 1)
          tmp = __utoa(va_arg(ap, unsigned long long), utoabuf, 10, false);
        else if (flags & 1 << 5 || flags & 1 << 6)
          tmp = __utoa(va_arg(ap, size_t), utoabuf, 10, false);
        else if (flags & 1 << 0)
          tmp = __utoa(va_arg(ap, unsigned long), utoabuf, 10, false);
        else if (flags & 1 << 2)
          tmp = __utoa((unsigned short)va_arg(ap, unsigned int), utoabuf, 10,
                       false);
        else if (flags & 1 << 3)
          tmp = __utoa((unsigned char)va_arg(ap, unsigned int), utoabuf, 10,
                       false);
        else
          tmp = __utoa(va_arg(ap, unsigned int), utoabuf, 10, false);
        argstrlen = strlen(tmp);
        if (!precisionset)
          precision = 1;
        else
          fillchar = ' ';
        if (!precision && tmp[0] == '0') {
          tmp = "";
          argstrlen = 0;
        }
        precision -= argstrlen;
        if (precision < 0)
          precision = 0;
        while (fill > argstrlen + precision) {
          if (j < size)
            str[j] = fillchar;
          j++;
          fill--;
        }
        argstrlen += precision;
        while (precision--) {
          if (j < size)
            str[j] = '0';
          j++;
        }
        while (tmp[k]) {
          if (j < size)
            str[j] = tmp[k];
          j++;
          k++;
        }
        fill += argstrlen;
        while (fill < 0) {
          if (j < size)
            str[j] = ' ';
          j++;
          fill++;
        }
        break;
      case 'O':
        flags |= 1 << 0;
        /* fall through */
      case 'o':
        if (flags & 1 << 4)
          tmp = __utoa(va_arg(ap, uintmax_t), utoabuf, 8, false);
        else if (flags & 1 << 1)
          tmp = __utoa(va_arg(ap, unsigned long long), utoabuf, 8, false);
        else if (flags & 1 << 5 || flags & 1 << 6)
          tmp = __utoa(va_arg(ap, size_t), utoabuf, 8, false);
        else if (flags & 1 << 0)
          tmp = __utoa(va_arg(ap, unsigned long), utoabuf, 8, false);
        else if (flags & 1 << 2)
          tmp = __utoa((unsigned short)va_arg(ap, unsigned int), utoabuf, 8,
                       false);
        else if (flags & 1 << 3)
          tmp = __utoa((unsigned char)va_arg(ap, unsigned int), utoabuf, 8,
                       false);
        else
          tmp = __utoa(va_arg(ap, unsigned int), utoabuf, 8, false);
        argstrlen = strlen(tmp);
        if (!precisionset)
          precision = 1;
        else
          fillchar = ' ';
        if (!precision && tmp[0] == '0') {
          tmp = "";
          argstrlen = 0;
        }
        precision -= argstrlen;
        if (precision < 0)
          precision = 0;
        if (altform && tmp[0] != '0' && !precision)
          precision = 1;
        while (fill > argstrlen + precision) {
          if (j < size)
            str[j] = fillchar;
          j++;
          fill--;
        }
        argstrlen += precision;
        while (precision--) {
          if (j < size)
            str[j] = '0';
          j++;
        }
        while (tmp[k]) {
          if (j < size)
            str[j] = tmp[k];
          j++;
          k++;
        }
        fill += argstrlen;
        while (fill < 0) {
          if (j < size)
            str[j] = ' ';
          j++;
          fill++;
        }
        break;
      case 'F':
      case 'f': {
        char ftoabuf[precision + 24];
        if (flags & 1 << 7)
          tmp = __ftoa(va_arg(ap, long double), precision, ftoabuf);
        else
          tmp = __ftoa(va_arg(ap, double), precision, ftoabuf);
        argstrlen = strlen(tmp);
        if (tmp[0] == '-') {
          sign = '-';
          tmp++;
          argstrlen--;
        }
        if (sign) {
          argstrlen++;
          if (fillchar == '0') {
            if (j < size)
              str[j] = sign;
            j++;
            sign = 0;
          }
        }
        while (fill > argstrlen) {
          if (j < size)
            str[j] = fillchar;
          j++;
          fill--;
        }
        if (sign) {
          if (j < size)
            str[j] = sign;
          j++;
        }
        while (tmp[k]) {
          if (j < size)
            str[j] = tmp[k];
          j++;
          k++;
        }
        fill += argstrlen;
        while (fill < 0) {
          if (j < size)
            str[j] = ' ';
          j++;
          fill++;
        }
      } break;
      case 'p':
        altform = true;
        flags = 1 << 5;
        /* fall through */
      case 'x':
        if (flags & 1 << 4)
          tmp = __utoa(va_arg(ap, uintmax_t), utoabuf, 16, false);
        else if (flags & 1 << 1)
          tmp = __utoa(va_arg(ap, unsigned long long), utoabuf, 16, false);
        else if (flags & 1 << 5 || flags & 1 << 6)
          tmp = __utoa(va_arg(ap, size_t), utoabuf, 16, false);
        else if (flags & 1 << 0)
          tmp = __utoa(va_arg(ap, unsigned long), utoabuf, 16, false);
        else if (flags & 1 << 2)
          tmp = __utoa((unsigned short)va_arg(ap, unsigned int), utoabuf, 16,
                       false);
        else if (flags & 1 << 3)
          tmp = __utoa((unsigned char)va_arg(ap, unsigned int), utoabuf, 16,
                       false);
        else
          tmp = __utoa(va_arg(ap, unsigned int), utoabuf, 16, false);
        argstrlen = strlen(tmp);
        if (tmp[0] == '0' && *fmt != 'p')
          altform = false;
        if (!precisionset)
          precision = 1;
        else
          fillchar = ' ';
        if (!precision && tmp[0] == '0') {
          tmp = "";
          argstrlen = 0;
        }
        precision -= argstrlen;
        if (precision < 0)
          precision = 0;
        if (altform) {
          argstrlen += 2;
          if (fillchar == '0') {
            if (j < size)
              str[j] = '0';
            j++;
            if (j < size)
              str[j] = 'x';
            j++;
            altform = false;
          }
        }
        while (fill > argstrlen + precision) {
          if (j < size)
            str[j] = fillchar;
          j++;
          fill--;
        }
        if (altform) {
          if (j < size)
            str[j] = '0';
          j++;
          if (j < size)
            str[j] = 'x';
          j++;
        }
        argstrlen += precision;
        while (precision--) {
          if (j < size)
            str[j] = '0';
          j++;
        }
        while (tmp[k]) {
          if (j < size)
            str[j] = tmp[k];
          j++;
          k++;
        }
        fill += argstrlen;
        while (fill < 0) {
          if (j < size)
            str[j] = ' ';
          j++;
          fill++;
        }
        break;
      case 'X':
        if (flags & 1 << 4)
          tmp = __utoa(va_arg(ap, uintmax_t), utoabuf, 16, true);
        else if (flags & 1 << 1)
          tmp = __utoa(va_arg(ap, unsigned long long), utoabuf, 16, true);
        else if (flags & 1 << 5 || flags & 1 << 6)
          tmp = __utoa(va_arg(ap, size_t), utoabuf, 16, true);
        else if (flags & 1 << 0)
          tmp = __utoa(va_arg(ap, unsigned long), utoabuf, 16, true);
        else if (flags & 1 << 2)
          tmp = __utoa((unsigned short)va_arg(ap, unsigned int), utoabuf, 16,
                       true);
        else if (flags & 1 << 3)
          tmp = __utoa((unsigned char)va_arg(ap, unsigned int), utoabuf, 16,
                       true);
        else
          tmp = __utoa(va_arg(ap, unsigned int), utoabuf, 16, true);
        argstrlen = strlen(tmp);
        if (tmp[0] == '0')
          altform = false;
        if (!precisionset)
          precision = 1;
        else
          fillchar = ' ';
        if (!precision && tmp[0] == '0') {
          tmp = "";
          argstrlen = 0;
        }
        precision -= argstrlen;
        if (precision < 0)
          precision = 0;
        if (altform) {
          argstrlen += 2;
          if (fillchar == '0') {
            if (j < size)
              str[j] = '0';
            j++;
            if (j < size)
              str[j] = 'X';
            j++;
            altform = false;
          }
        }
        while (fill > argstrlen + precision) {
          if (j < size)
            str[j] = fillchar;
          j++;
          fill--;
        }
        if (altform) {
          if (j < size)
            str[j] = '0';
          j++;
          if (j < size)
            str[j] = 'X';
          j++;
        }
        argstrlen += precision;
        while (precision--) {
          if (j < size)
            str[j] = '0';
          j++;
        }
        while (tmp[k]) {
          if (j < size)
            str[j] = tmp[k];
          j++;
          k++;
        }
        fill += argstrlen;
        while (fill < 0) {
          if (j < size)
            str[j] = ' ';
          j++;
          fill++;
        }
        break;
      case 'm':
        tmp = strerror(errno);
        argstrlen = strlen(tmp);
        while (fill > argstrlen) {
          if (j < size)
            str[j] = fillchar;
          j++;
          fill--;
        }
        while (tmp[k]) {
          if (j < size)
            str[j] = tmp[k];
          j++;
          k++;
        }
        fill += argstrlen;
        while (fill < 0) {
          if (j < size)
            str[j] = ' ';
          j++;
          fill++;
        }
        break;
      case 'n':
        if (flags & 1 << 4)
          *va_arg(ap, uintmax_t *) = j;
        else if (flags & 1 << 1)
          *va_arg(ap, unsigned long long *) = j;
        else if (flags & 1 << 5 || flags & 1 << 6)
          *va_arg(ap, size_t *) = j;
        else if (flags & 1 << 0)
          *va_arg(ap, unsigned long *) = j;
        else if (flags & 1 << 2)
          *va_arg(ap, unsigned short *) = j;
        else if (flags & 1 << 3)
          *va_arg(ap, unsigned char *) = j;
        else
          *va_arg(ap, unsigned int *) = j;
        break;
      default:
        fmt--;
        break;
      }
      i += fmt - fmt2 + 1;
    } else {
      if (j < size)
        str[j] = format[i];
      j++;
      i++;
    }
  }
  if (j < size)
    str[j] = '\0';
  else if (size > 0)
    str[size - 1] = '\0';
  return j;
}