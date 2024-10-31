#ifndef TYPES_H
#define TYPES_H
typedef enum {
  untyped, i32, f32
} type;

typedef struct {
  type t;
  union {
    int32_t val_i32;
    float   val_f32;
  };
} typed_val;


typedef void (writer_t)(FILE *f, void *);

void __write_f32(FILE *f, void *x_p) {
  fprintf(f, "%f", *(float*) x_p);
}
void __write_i32(FILE *f, void *x_p) {
  fprintf(f, "%d", *(int32_t*) x_p);
}

typedef struct {
  writer_t *writer;
} type_info_t;

type_info_t float_type_info =
  { .writer = __write_f32 } ;

#endif // TYPES_H
