#include "matrix.h"
#include <stdio.h>
#include <stdlib.h>

Matrix matrix_new(size_t h, size_t w) {
    Matrix mat;
    mat.w = w;
    mat.h = h;

    // Allocate matrix
    mat.val = (float **) malloc(h * sizeof(float *));

    // Allocate matrix rows on the heap and intialize them to 0
    for (size_t i = 0; i < h; i++) {
        mat.val[i] = (float *) calloc(w, sizeof(float));
    }

    return mat;
}

void matrix_free(Matrix *mat) {
    if (!mat->val)
        return;

    // Free matrix rows
    for (size_t i = 0; i < mat->h; i++) {
        free(mat->val[i]);
    }

    // Free the matrix itself
    free(mat->val);
}

void matrix_print(Matrix mat) {
    if (!mat.val)
        return;

    for (size_t i = 0; i < mat.h; i++) {
        for (size_t j = 0; j < mat.w; j++) {
            printf("%f ", mat.val[i][j]);
        }
        printf("\n");
    }
}

void matrix_randomize(Matrix *mat) {
    if (!mat->val)
        return;

    for (size_t i = 0; i < mat->h; i++) {
        for (size_t j = 0; j < mat->w; j++) {
            mat->val[i][j] = 2 * ((float) rand() / RAND_MAX) - 1;
        }
    }
}

void matrix_scale(Matrix *mat, float scalar) {
    for(size_t i = 0; i<mat->h; i++){
        for(size_t j = 0; j<mat->w; j++) {
            mat->val[i][j] *= scalar;
        }
    }    
}