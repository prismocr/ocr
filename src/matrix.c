#include "matrix.h"
#include "error.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

int matrix_new(size_t h, size_t w, Matrix *mat) {
    assert(h > 0 && w > 0);
    mat->w = w;
    mat->h = h;

    // Allocate matrix
    mat->val = (float **) malloc(h * sizeof(float *));
    if (mat->val == NULL) {
        set_last_errorf("Failed to allocated memory for matrix: %s",
            strerror(errno));
        return 1;
    }

    // Allocate matrix rows on the heap and intialize them to 0
    for (size_t i = 0; i < h; i++) {
        mat->val[i] = (float *) calloc(w, sizeof(float));
        if (mat->val[i] == NULL) {
            set_last_errorf("Failed to allocated memory for matrix: %s",
                strerror(errno));
            return 1;
        }
    }

    return 0;
}

int matrix_copy(Matrix mat, Matrix *copy) {
    assert(mat.val != NULL);
    size_t i, j;

    if (matrix_new(mat.h, mat.w, copy)) {
        return 1;
    }
    for (i = 0; i < mat.h; i++) {
        for (j = 0; j < mat.w; j++) {
            copy->val[i][j] = mat.val[i][j];
        }
    }

    return 0;
}

void matrix_free(Matrix *mat) {
    assert(mat->val != NULL);

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

float matrix_average(Matrix mat) {
    assert(mat.val != NULL);
    size_t i, j;

    float sum = 0;
    for (i = 0; i < mat.h; i++) {
        for (j = 0; j < mat.w; j++) {
            sum += mat.val[i][j];
        }
    }

    return sum / (mat.h + mat.w);
}

