#include "array_t.h"
#include "parsing.h"
#include <signal.h>

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define ERROR(msg) {fprintf(stderr, (msg)); i = FAULTY_INPUT; break;}

static void next_token(FILE *in, char *buf) {

  skip_whitespace(in);

  int8_t c;
  uint8_t i = 0;
  while (i < BUF_SIZE - 1) {

    buf[i] = c = getc(in);

    if (c == EOF) {
      buf[i] = 0;
      return;
    }

    else if (!is_constituent(c)) {
      if (i == 0) {
        buf[i+1] = 0;
      }
      else {
        ungetc(c, in);
        buf[i] = 0;
      }
      return;
    }
    i++;
  }
}

static int64_t read_array(FILE *in, array_t *arr) {

  char buf[BUF_SIZE];
  buf[BUF_SIZE-1] = '\0';

  void     *out  = arr->data;
  uint64_t *dims = arr->dims;

  uint64_t *tmp_dims = Calloc(NUM_DIMS_MAX, sizeof(uint64_t));
  uint8_t  *visited  = Calloc(NUM_DIMS_MAX, sizeof(uint8_t));

  int64_t len = ARR_LEN_INIT;
  int64_t i   = 0;

  int8_t current_dim = -1;
  int8_t inner_dim   = -1;

  TYPE tmp_read_val;

  while (1) {
    next_token(in, buf);

    if (strlen(buf) == 0) {
      if (current_dim > -1) {
        ERROR("Unexpected EOF!\n");
      }
      break;
    }

    else if (streq (buf, "]")) {

      if (current_dim < 0) {
        ERROR("Unexpected closing bracket!\n");
      }


      // if dim already visited, assert that size equals expected size.
      else if (visited[current_dim] &&
               tmp_dims[current_dim] != dims[current_dim]) {
        ERROR("Input array is irregular!\n");
      }

      // if dim not yet visited ..
      else {
        // record its size and note that it has been visited.
        dims[current_dim] = tmp_dims[current_dim];
        visited[current_dim] = 1;
      }

      current_dim--;
    }

    else if (streq(buf, ",")) {
      // TODO: should probably comma-separate values?
    }

    else if (streq(buf, "[")) {

      // entering dim.
      if (current_dim >= inner_dim && inner_dim > -1) {
        ERROR("Non-scalar value (ie. array) in innermost dimension!\n");
      }

      else if (current_dim >= NUM_DIMS_MAX - 1) {
        ERROR("Too many dimensions in input! (max 127)\n");
      }

      if (current_dim > -1) tmp_dims[current_dim]++;

      tmp_dims[++current_dim] = 0;
    }

    // attempt to parse a value
    else if (sscanf(buf, FORMAT, &tmp_read_val) == 1) {
      if (current_dim < inner_dim) {
        ERROR("Scalar values in some non-innermost dimension!\n");
      }

      ((TYPE*)out)[i++] = tmp_read_val;

      if (i >= len) {
        len *= 2;
        out = Realloc(out, len * sizeof(TYPE));
      }

      inner_dim = current_dim;
      tmp_dims[current_dim]++;
    }

    else {
      ERROR("Unrecognized token in input!\n");
    }

    if (current_dim < 0) { 
      // we have reached the end of this input array, but for now, the data
      // reader will not stop until an EOF is read.
      // break;
    }
  }


  free(tmp_dims);
  free(visited);

  if (i >= 0) {
    arr->num_dims = MAX(inner_dim + 1, 1);
    arr->data = Realloc(arr->data, i * sizeof(void*));
    arr->dims = Realloc(arr->dims, arr->num_dims * sizeof(uint64_t));
  }
  return i;
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

  array_t *arr     = init_array(ARR_LEN_INIT);
  int64_t num_read = read_array(file, arr);

  if (num_read >= 0)
    print_array(arr);
  else
    printf(">> Error! Read nothing.\n");

  free_array(arr);
}
