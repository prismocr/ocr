#ifndef VECTOR_H
#define VECTOR_H

typedef struct Vector Vector;
struct Vector {
    size_t size;
    float *values;
};

int vector_new(size_t size, Vector *Vector);
void vector_free(Vector *vector);

void vector_print(Vector vector);
void vector_printf(const char *format, Vector vector);

void vector_randomize(Vector *vect, float min, float max);

void vector_add(Vector a, Vector b, Vector *res);
void vector_sub(Vector a, Vector b, Vector *res);
void vector_scale(Vector vect, float scalar, Vector *res);
float vector_dot(Vector a, Vector b);

Vector array_to_vector(float *array, size_t size);

#endif