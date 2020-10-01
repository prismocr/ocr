#include <stdio.h>
#include <stdlib.h>
#include "matrix.h"

Matrix matrix_new(size_t w, size_t h) {
    Matrix mat;
    mat.w = w;
    mat.h = h;

    // Dynamic allocation of first dim
    mat.val = (float **) malloc(h * sizeof(float *));
    // Dynamic allocation of second dim with initialization to 0
    for (size_t i = 0; i < h; i++) {
        mat.val[i] = (float *) calloc(w, sizeof(float));
    }

    return mat;
}

void matrix_free(Matrix *mat) {
    // Freeing every line of the matrix
    for (size_t i = 0; i < mat->h; i++) {
        free(mat->val[i]);
    }
    // Freeing the matrix itself
    free(mat->val);
}

void matrix_print(Matrix mat) {
    for (size_t i = 0; i < mat.h; i++) {
        for (size_t j = 0; j < mat.w; j++) {
            printf("%f ", mat.val[i][j]);
        }
        printf("\n");
    }
}

void matrix_randomize(Matrix *mat) {
    for (size_t i = 0; i < mat->h; i++) {
        for (size_t j = 0; j < mat->w; j++) {
            mat->val[i][j] = 2 * ((float) rand() / RAND_MAX) - 1;
        }
    }
}
