#ifndef MATRIX_H
#define MATRIX_H

#include <stddef.h>

typedef struct matrix Matrix;

struct matrix {
    size_t h, w;
    float **val;
};

// Creates a new matrix of width @w and height @h filled with 0
Matrix matrix_new(size_t w, size_t h);

// Frees dynamically allocated array inside matrix struct
void matrix_free(Matrix *mat);

// Prints a matrix
void matrix_print(Matrix mat);

// Randomizes a matrix with floats between 0 and 1
void matrix_randomize(Matrix *mat);

void matrix_scale(Matrix *mat, float scalar);

#endif // MATRIX_H
