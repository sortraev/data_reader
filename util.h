#pragma once
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
#define NUM_DIMS_INIT 2

#define FAULTY_INPUT -1
#define WHERE_PRINT  stdout


#ifndef FLOAT
#define FLOAT 1
#endif
#if FLOAT
typedef double TYPE;
#define FORMAT "%lf"
#else
typedef int64_t TYPE;
#define FORMAT "%ld"
#endif


void *Realloc(void *old_ptr, size_t alloc_size) {

  void *r = realloc(old_ptr, alloc_size);
  if (r == NULL && alloc_size > 0) {
    fprintf(stderr, "realloc() error: %s\n", strerror(errno));
    exit(1);
  }
  return r;
}


// reallocate, and if new allocation is larger than before; zero-initialize
// additional bytes.
void *Recalloc(void *old_ptr, size_t old_size, size_t num_elems, size_t elem_size) {

  size_t new_size = num_elems * elem_size; // TODO: catch and handle overflow.
  void *r         = Realloc(old_ptr, new_size);

  if (new_size > old_size) {
    char   *uninitialized_start = (char*) r + old_size;
    size_t num_uninitialized    = new_size - old_size;

    for (size_t i = 0; i < num_uninitialized; i++)
      uninitialized_start[i] = 0;
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


// TODO: find another place for these. perhaps a types.h?
typedef union {
  int64_t i;
  double  d;
} val;

typedef enum {
  untyped, i32, i64, f32, f64
} type;

typedef struct {
  val  v;
  type t;
} typed_value;
