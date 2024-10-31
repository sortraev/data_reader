#pragma once
#include "util.h"

#include "types.h"

int streq(char *x, char *y) {
  return strncmp(x, y, strlen(y)) == 0;
}

int is_num(int c) {
  return ((char) c >= '0') && ((char) c <= '9');
}

int is_constituent(int c) {
  char _c = (char) c;
  return is_num(c) || _c == '-' || _c == '.' || _c == 'e' || _c == 'E';
}

int is_whitespace(int c) {
  char _c = (char) c;
  return _c == ' '                  // space
     || (_c >= '\t' && _c <= '\r'); // \t, \n, \v, \f, \r
}

void skip_whitespace(FILE *f) {
  int c;
  while (is_whitespace(c = getc(f)));
  if (c != EOF)
    assert(ungetc(c, f) != EOF);
}


int parse_int(char *buf, typed_val *tv) {
  char *endptr;
  tv->val_i32 = strtol(buf, &endptr, 10);
  if (!*endptr) {
    tv->t = i32;
    return 0;
  }
  return 1;
}

int parse_float(char *buf, typed_val *tv) {
  char *endptr;
  tv->val_f32 = (float) strtod(buf, &endptr);
  if (*endptr == '\0' || (memcmp(endptr, "f32\0", 4) == 0)) {
    tv->t = f32;
    return 0;
  }
  return 1;
}

int parse(char *buf, typed_val *tv) {
  int ret = !(parse_int(buf, tv) == 0 || parse_float(buf, tv) == 0);
  return ret;
}
