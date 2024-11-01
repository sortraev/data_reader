#ifndef ARRAY_T_H
#define ARRAY_T_H

#include "util.h"
#include "types.h"

typedef struct {
  uint64_t size;
  uint8_t  known;
} dim;

typedef struct array_t {
  void     *data;
  uint64_t *dims;
  int32_t   num_dims;
  type      elem_t;
} array_t;

array_t *array_init(uint64_t len, size_t elem_size) {

  array_t *arr  = Malloc(sizeof(array_t));
  arr->data     = Malloc(len * elem_size);
  arr->dims     = Calloc(NUM_DIMS_MAX, sizeof(uint64_t));
  arr->num_dims = 0;

  return arr;
}

static uint64_t dims_size(uint64_t *dims, uint8_t start_dim, uint8_t num_dims) {
  uint64_t acc = 1;
  for (uint32_t i = start_dim; i < num_dims; i++) {
    acc *= dims[i];
  }
  return acc;
}

void array_destroy(array_t *arr) {
  if (arr) {
    free(arr->data);
    free(arr->dims);
  }
  free(arr);
}

uint64_t array_len(array_t *arr) {
  return dims_size(arr->dims, 0, arr->num_dims);
}

void __array_print(void     *data,
                   uint64_t *dims,
                   int8_t    this_dim,
                   int8_t    dims_left,
                   uint64_t  offset) {

  if (this_dim < 0) {
    // TODO: does this edge case need to be caught..? don't think so.
    return;
  }
  uint64_t size_this_dim   = dims[this_dim];
  uint64_t size_inner_dims = dims_left > 0 ?
                             dims_size(dims, this_dim+1, this_dim+dims_left) : 0;

  fprintf(WHERE_PRINT, "[");
  for (uint64_t i = 0; i < size_this_dim; i++) {
    if (dims_left <= 1) {
      fprintf(WHERE_PRINT, FORMAT, ((TYPE*)data)[offset+i]);
    }
    else {
      __array_print(data, dims,
                    this_dim+1, dims_left-1,
                    offset + i*size_inner_dims);
    }

    if (i < size_this_dim - 1) {
      fprintf(WHERE_PRINT, ", ");
    }
  }

  fprintf(WHERE_PRINT, "]");
}

void array_print(array_t *arr) {

  uint64_t *dims     = arr->dims;
  void     *data     = arr->data;
  uint8_t   num_dims = arr->num_dims;

  fprintf(WHERE_PRINT, ">> ");

  if (num_dims == 0) {
    fprintf(WHERE_PRINT, FORMAT, ((TYPE*)data)[0]);
  }
  else {

    uint8_t  start_dim   = 0;
    uint64_t data_offset = 0;

    __array_print(data, dims, start_dim, num_dims, data_offset);
  }
  fprintf(WHERE_PRINT, "\n");
}



void array_print_flat(array_t *arr, int len) {

  int64_t *data = (int64_t*) arr->data;

  printf(">> [");
  for (int i = 0; i < len-1; i++) {
    printf("%ld, ", data[i]);
  }
  if (len > 0)
    printf("%ld]\n", data[len-1]);
  else
    printf("]\n");
}
#endif // ARRAY_T_H
