#ifndef MATRIX_H
#define MATRIX_H

#include <stddef.h>
#include <stdbool.h>

typedef struct matrix Matrix;

struct matrix {
    size_t h, w;
    float **val;
};

/**
 * Initializes a new matrix of width @w and height @h filled with 0
 *
 * @param height of the matrix
 * @param width of the matrix
 * @param matrix to initialize
 * @return state (error)
 */
int matrix_new(size_t h, size_t w, Matrix *mat);

/**
 * Copies a matrix @mat into a matrix @copy
 *
 * @param matrix to copy from
 * @param matrix to copy into
 * @return state (error)
 */
int matrix_copy(Matrix mat, Matrix *copy);

/**
 * Frees dynamically allocated array inside matrix struct
 *
 * @param matrix to free
 */
void matrix_free(Matrix *mat);

/**
 * Prints a matrix
 *
 * @param matrix to print
 */
void matrix_print(Matrix mat);

/**
 * Prints each element of the matrix with the given format
 *
 * @param matrix to print
 */
void matrix_printf(const char *elem_fmt, Matrix mat);

/**
 * Fill matrix @mat with random value between @min and @max
 *
 * @param minimum value for randomizing
 * @param maximum value for randomizing
 * @param matrix to randomize
 */

bool matrix_equal(Matrix mat1, Matrix mat2);

// Fill matrix with random value between min and max
void matrix_randomize(float min, float max, Matrix *mat);

/**
 * Apply a scalar on a matrix
 *
 * @param matrix to apply scalar on
 * @param scalar to apply on the matrix
 */
void matrix_scale(Matrix *mat, float scalar);

/**
 * Adds a constant on each and every element of the matrix
 *
 * @param matrix
 * @param value to add on each element of the matrix
 */
void matrix_add_const(Matrix *mat, float c);

/**
 * Returns the average of the matrix values
 *
 * @param the matrix
 * @return average value of @mat
 */
float matrix_average(Matrix mat);

// ----- Matrix operations ---
/**
 * Apply a dot product between twos matrices @mat_a and @mat_b
 *
 * @param first matrix
 * @param second matrix
 * @param reference to save the result in
 */
void matrix_dot(Matrix mat_a, Matrix mat_b, Matrix *res);

/**
 * Apply a dot product between a matrix and an array (considered as a column)
 *
 * @param the matrix
 * @param the column
 * @param the array to save the result in
 */
void matrix_column_dot(Matrix mat_a, float *column, float *res);

/**
 * Apply a dot product between a matrix and an array (considered as a column)
 * Uses the matrix as if it was transposed
 * The result does not overwrite the content of @res, it adds to it
 *
 * @param the matrix
 * @param the column
 * @param the array to save the result in
 */
void matrix_cdt(Matrix mat_a, float *column, float *res);

/**
 * Apply a dot product between a column and a line, resulting in a matrix
 * The result does not overwrite the content of @res, it adds to it
 *
 * @param number of line of the column @param a
 * @param the column
 * @param number of columns of the line @param b
 * @param the line
 * @param matrix to save the result in
 */
void matrix_dcl(size_t nb_lin, float *a, size_t nb_col, float *b, Matrix *mat);

#endif // MATRIX_H
