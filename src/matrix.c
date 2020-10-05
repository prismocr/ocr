#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "matrix.h"

Matrix matrix_new(size_t h, size_t w) {
    assert(h > 0 && w > 0);
    Matrix mat;
    mat.h = h;
    mat.w = w;

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
    assert(mat.val != NULL);
    size_t i, j;

    for (i = 0; i < mat.h; i++) {
        for (j = 0; j < mat.w; j++) {
            printf("%f ", mat.val[i][j]);
        }
        printf("\n");
    }
}

void matrix_printf(const char *elem_fmt, Matrix mat) {
    assert(mat.val != NULL);
    size_t i, j;

    for (i = 0; i < mat.h; i++) {
        for (j = 0; j < mat.w; j++) {
            printf(elem_fmt, mat.val[i][j]);
        }
        printf("\n");
    }
}

void matrix_randomize(float min, float max, Matrix *mat) {
    assert(min < max);
    assert(mat->val != NULL);
    size_t i, j;

    for (i = 0; i < mat->h; i++) {
        for (j = 0; j < mat->w; j++) {
            mat->val[i][j] = ((float) rand() / RAND_MAX) * (max - min) + min;
        }
    }
}

Matrix matrix_dot(Matrix mat_a, Matrix mat_b) {
    Matrix res = matrix_new(mat_a.h, mat_b.w);

    // for each row of mat_a
    for (size_t i = 0; i < mat_a.h; i++) {
        // for each column of mat_b
        for (size_t j = 0; j < mat_b.w; j++) {
            // for each column of mat_a
            for (size_t k = 0; k < mat_a.w; k++) {
                res.val[i][j] += mat_a.val[i][k] * mat_b.val[k][j];
            }
        }
    }

    return res;
}

void matrix_column_dot(Matrix mat_a, float *column, float *res) {
    // for each row of mat_a
    for (size_t i = 0; i < mat_a.h; i++) {
        res[i] = 0.0f;
        // for each column of mat_a
        for (size_t j = 0; j < mat_a.w; j++) {
            res[i] += mat_a.val[i][j] * column[j];
        }
    }
}