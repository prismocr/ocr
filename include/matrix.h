#ifndef MATRIX_H
#define MATRIX_H

#include <stddef.h>

typedef struct matrix Matrix;

struct matrix {
    size_t h, w;
    float **val;
};

// Creates a new matrix of width @w and height @h filled with 0
Matrix matrix_new(size_t h, size_t w);

Matrix matrix_copy(Matrix mat);

// Frees dynamically allocated array inside matrix struct
void matrix_free(Matrix *mat);

// Prints a matrix
void matrix_print(Matrix mat);

// Prints each element of the matrix with the given format
void matrix_printf(const char *elem_fmt, Matrix mat);

// Fill matrix with random value between min and max
void matrix_randomize(float min, float max, Matrix *mat);

#endif // MATRIX_H