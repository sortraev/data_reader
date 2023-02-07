#pragma once
#include "util.h"

int streq(char *x, char *y) {
  return strcmp(x, y) == 0;
}

int is_num(int c) {
  return ((char) c >= '0') && ((char) c <= '9');
}

int is_whitespace(int c) {
  char _c = (char) c;
  return _c == ' '                  // space
     || (_c >= '\t' && _c <= '\r'); // \t, \n, \v, \f, \r
}

int is_constituent (int c) {
  char _c = (char) c;
  return is_num(c) || _c == '-' || _c == '.' || _c == 'e' || _c == 'E';
}


void skip_whitespace(FILE *in) {
  int c;
  while (is_whitespace(c = getc(in)));
  if (c != EOF)
    assert(ungetc(c, in) != EOF);
}


int parse_int(char *buf, typed_value *tv) {
  char *endptr;
  tv->v.i = strtol(buf, &endptr, 10);
  if (!*endptr) {
    tv->t = i64;
    return 0;
  }
  return 1;
}

int parse_double(char *buf, typed_value *tv) {
  char *endptr;
  tv->v.d = strtod(buf, &endptr);
  if (!*endptr) {
    tv->t = f64;
    return 0;
  }
  return 1;
}

int parse(char *buf, typed_value *tv) {
  int ret = !(parse_int(buf, tv) == 0 || parse_double(buf, tv) == 0);
  return ret;
}


