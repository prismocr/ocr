#ifndef VECTOR_H
#define VECTOR_H

typedef struct Vector Vector;
struct Vector {
    size_t size;
    float *val;
};

int vector_new(size_t size, Vector *Vector);
void vector_free(Vector *vector);

void vector_print(Vector vector);
void vector_printf(const char *format, Vector vector);

void vector_randomize(Vector *vect, float min, float max);

void vector_add(size_t size, float *a, float *b);
void vector_sub(size_t size, float *a, float *res);
void vector_scale(size_t size, float scalar, float *arr);
void vector_dot(size_t size, float *a, float *b);

Vector arr2vect(float *array, size_t size);
void vector_copy(size_t size, float *a, float *b);

#endif