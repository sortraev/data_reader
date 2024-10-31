#include "array_t.h"
#include "parsing.h"
#include "types.h"
#include <signal.h>

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define ERROR(msg) {fprintf(stderr, (msg)); status = FAULTY_INPUT; break;}

static int next_token(FILE *f, char *buf, const int buf_size) {

  skip_whitespace(f);

  int8_t c;
  uint8_t i = 0;
  while (i < buf_size - 1) {

    buf[i] = c = getc(f);

    if (c == EOF) {
      buf[i] = 0;
      return EOF;
    }

    else if (!is_constituent(c)) {
      if (i == 0) {
        buf[i+1] = 0;
      }
      else {
        ungetc(c, f);
        buf[i] = 0;
      }
      break;
    }
    i++;
  }
  return 0;
}

static array_t *read_array(FILE *f) {

  array_t *arr = array_init(ARR_LEN_INIT, sizeof(float)); // TODO: generalize.

  static char buf[BUF_SIZE];
  buf[BUF_SIZE - 1] = '\0';

  void     *out  = arr->data;
  uint64_t *dims = arr->dims;

  uint64_t *tmp_dims = Calloc(NUM_DIMS_MAX, sizeof(uint64_t));
  uint8_t  *visited  = Calloc(NUM_DIMS_MAX, sizeof(uint8_t));

  uint64_t len = ARR_LEN_INIT;
  uint64_t i   = 0;

  int32_t current_dim = -1;
  int32_t inner_dim   = -1;

  typed_val tv;

  int status = 0;

  do {
    if (next_token(f, buf, BUF_SIZE) == EOF || *buf == '\0') {
      if (current_dim > -1) {
        ERROR("Unexpected EOF!\n");
      }
      else {
        status = EOF;
        break;
      }
    }

    else if (streq(buf, "[")) {
      /* 
       * entering array dimension.
       */

      if (current_dim >= NUM_DIMS_MAX - 1) {
        ERROR("Too many dimensions in input! (max 127)\n");
      }
      else if (inner_dim >= 0 && current_dim >= inner_dim) {
        ERROR("Non-scalar value (i.e. array) in innermost dimension!\n");
      }

      if (current_dim >= 0) {
        tmp_dims[current_dim]++;
      }
      current_dim++;
      tmp_dims[current_dim] = 0;
    }

    else if (streq(buf, "]")) {
      /*
       * exiting array dimension.
       */
      if (current_dim < 0) {
        ERROR("Unexpected closing bracket!\n");
      }

      // if dim already visited, assert that size equals expected size.
      else if (visited[current_dim]) {
        if (tmp_dims[current_dim] != dims[current_dim]) {
          ERROR("Input array is irregular!\n");
        }
      }

      // if dim not yet visited ..
      else {
        // record its size and note that it has been visited.
        dims[current_dim] = tmp_dims[current_dim];
        visited[current_dim] = 1;

        // typically we find the innermost dim when we find the first array
        // element. if this hasn't been found when we exit a dim, this means the
        // entire array must be empty (or ill-formed).
        if (inner_dim < 0) {
          inner_dim = current_dim;
        }
      }

      current_dim--;
    }

    else if (streq(buf, ",")) {
      // TODO: should probably comma-separate values?
    }

    // attempt to parse a value
    else {
      if (parse_float(buf, &tv) == 0) {
        if (current_dim < inner_dim) {
          ERROR("Scalar values in some non-innermost dimension!\n");
        }

        ((TYPE*)out)[i++] = tv.val_f32;

        if (i >= len) {
          out = Recalloc(out, len, len * 2, sizeof(TYPE));
          len *= 2;
        }

        if (current_dim >= 0) {
          inner_dim = current_dim;
          tmp_dims[current_dim]++;
        }
      }

      else {
        ERROR("Unrecognized token in input!\n");
      }
    }

  } while (current_dim >= 0);

  free(tmp_dims);
  free(visited);

  if (status != 0) {
    array_destroy(arr);
    return NULL;
  }

  arr->num_dims = MAX(inner_dim + 1, 0);
  arr->data = Realloc(out, i * sizeof(float)); // TODO: generalize.
  arr->dims = Realloc(dims, arr->num_dims * sizeof(uint64_t));
  return arr;
}

array_t *read_many_arrays(FILE *f) {
  array_t *arrs = array_init(ARR_LEN_INIT, sizeof(array_t*));
  arrs->num_dims = 1;

  array_t *arr;
  int i = 0;
  while ((arr = read_array(f))) {
    ((array_t**)arrs->data)[i] = arr;
    i++;
  }

  arrs->dims[0] = i;
  arrs->data = Realloc(arrs->data, i * sizeof(array_t*));
  arrs->dims = Realloc(arrs->dims, 1 * sizeof(uint64_t));

  return arrs;
}

int main(int argc, char **argv) {

  if (argc > 2) {
    fprintf(stderr, "Usage: %s <optional input file>\n", argv[0]);
    exit(1);
  }

  FILE *file = stdin;

  if (argc == 2 && (file = fopen(argv[1], "r")) == NULL) {
    fprintf(stderr, "Error opening file (probably nonexisting)\n");
    exit(1);
  }


  array_t *array_of_arrs = read_many_arrays(file);
  array_t **arrs = array_of_arrs->data;

  printf(">> Read %ld values:\n", array_of_arrs->dims[0]);
  for (size_t i = 0; i < array_of_arrs->dims[0]; i++) {
    array_print(arrs[i]);
    array_destroy(arrs[i]);
  }
  array_destroy(array_of_arrs);
}
