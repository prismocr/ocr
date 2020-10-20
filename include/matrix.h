#ifndef MATRIX_H
#define MATRIX_H

#include <stddef.h>
#include <stdbool.h>

typedef struct matrix Matrix;

struct matrix {
    size_t h, w;
    float **val;
};

// Creates a new matrix of width @w and height @h filled with 0
int matrix_new(size_t h, size_t w, Matrix *mat);

int matrix_copy(Matrix mat, Matrix *copy);

// Frees dynamically allocated array inside matrix struct
void matrix_free(Matrix *mat);

// Prints a matrix
void matrix_print(Matrix mat);

// Prints each element of the matrix with the given format
void matrix_printf(const char *elem_fmt, Matrix mat);

bool matrix_equal(Matrix mat1, Matrix mat2);

// Fill matrix with random value between min and max
void matrix_randomize(float min, float max, Matrix *mat);

void matrix_scale(Matrix *mat, float scalar);

// Returns the average of the matrix values
float matrix_average(Matrix mat);

// ----- Matrix operations ---
void matrix_dot(Matrix mat_a, Matrix mat_b, Matrix *res);
void matrix_column_dot(Matrix mat_a, float *column, float *res);
// matrix column dot transposed
void matrix_cdt(Matrix mat_a, float *column, float *res);
// matrix dot column-line
void matrix_dcl(size_t nb_lin, float *a, size_t nb_col, float *b, Matrix *mat);

#endif // MATRIX_H
