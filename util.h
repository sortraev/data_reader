#ifndef UTIL_H
#define UTIL_H
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <assert.h>
#include <limits.h>
#include <stdint.h>

#define BUF_SIZE      32
#define ARR_LEN_INIT  32

#define NUM_DIMS_MAX  CHAR_MAX
#define NUM_DIMS_INIT 1

#define FAULTY_INPUT -1
#define WHERE_PRINT  stdout

#ifndef FLOAT
#define FLOAT 1
#endif

#if FLOAT
typedef float TYPE;
#define FORMAT "%f"
#else
typedef int32_t TYPE;
#define FORMAT "%d"
#endif

void *Realloc(void *old_ptr, size_t alloc_size) {
  if (alloc_size == 0) {
    free(old_ptr);
    return NULL;
  }
  void *r = realloc(old_ptr, alloc_size);
  if (r == NULL) {
    fprintf(stderr, "realloc() error: %s\n", strerror(errno));
    exit(1);
  }
  return r;
}


void *Malloc(size_t size) {
  void *r = malloc(size);
  if (r == NULL && size > 0) {
    fprintf(stderr, "malloc() error: %s\n", strerror(errno));
    exit(1);
  }
  return r;
}

void *Calloc(size_t num_elems, size_t elem_size) {
  void *r = calloc(num_elems, elem_size);

  if (r == NULL && num_elems > 0 && elem_size > 0) {
    fprintf(stderr, "calloc() error: %s\n", strerror(errno));
    exit(1);
  }
  return r;
}

void *Recalloc(void *old_ptr, size_t old_num, size_t new_num, size_t elem_size) {
  void *r = Calloc(new_num, elem_size);
  memcpy(r, old_ptr, old_num * elem_size);
  free(old_ptr);
  return r;
}

#endif // UTIL_H
