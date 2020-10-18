#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include "matrix.h"
#include "error.h"

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

    if (matrix_new(mat.h, mat.w, copy)) {
        return 1;
    }
    for (size_t i = 0; i < mat.h; i++) {
        for (size_t j = 0; j < mat.w; j++) {
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

    for (size_t i = 0; i < mat.h; i++) {
        for (size_t j = 0; j < mat.w; j++) {
            printf("%f ", mat.val[i][j]);
        }
        printf("\n");
    }
}

void matrix_printf(const char *elem_fmt, Matrix mat) {
    assert(mat.val != NULL);

    for (size_t i = 0; i < mat.h; i++) {
        for (size_t j = 0; j < mat.w; j++) {
            printf(elem_fmt, mat.val[i][j]);
        }
        printf("\n");
    }
}

void matrix_randomize(float min, float max, Matrix *mat) {
    assert(min < max);
    assert(mat->val != NULL);

    for (size_t i = 0; i < mat->h; i++) {
        for (size_t j = 0; j < mat->w; j++) {
            mat->val[i][j] = ((float) rand() / RAND_MAX) * (max - min) + min;
        }
    }
}

void matrix_scale(Matrix *mat, float scalar) {
    assert(mat->val != NULL);

    for (size_t i = 0; i < mat->h; i++) {
        for (size_t j = 0; j < mat->w; j++) {
            mat->val[i][j] *= scalar;
        }
    }
}

float matrix_average(Matrix mat) {
    assert(mat.val != NULL);

    float sum = 0;
    for (size_t i = 0; i < mat.h; i++) {
        for (size_t j = 0; j < mat.w; j++) {
            sum += mat.val[i][j];
        }
    }

    return sum / (mat.h + mat.w);
}

void matrix_dot(Matrix mat_a, Matrix mat_b, Matrix *res) {
    assert(mat_a.w == mat_b.h);
    assert(mat_a.val != NULL);
    assert(mat_b.val != NULL);
    matrix_new(mat_a.h, mat_b.w, res);

    // for each row of mat_a
    for (size_t i = 0; i < mat_a.h; i++) {
        // for each column of mat_b
        for (size_t j = 0; j < mat_b.w; j++) {
            // for each column of mat_a
            for (size_t k = 0; k < mat_a.w; k++) {
                res->val[i][j] += mat_a.val[i][k] * mat_b.val[k][j];
            }
        }
    }
}

void matrix_column_dot(Matrix mat_a, float *column, float *res) {
    // WARN: You cannot check array size from a pointer
    // assert(mat_a.w == sizeof(column) / sizeof(column[0]);

    // for each row of mat_a
    for (size_t i = 0; i < mat_a.h; i++) {
        res[i] = 0.0f;
        // for each column of mat_a
        for (size_t j = 0; j < mat_a.w; j++) {
            res[i] += mat_a.val[i][j] * column[j];
        }
    }
}

// Dot product between a matrix @mat_a and a @column.Result is transposed.
void matrix_cdt(Matrix mat_a, float *column, float *res) {
    // for each column of mat_a
    for (size_t i = 0; i < mat_a.w; i++) {
        res[i] = 0.0f;
        // for each row of mat_a
        for (size_t j = 0; j < mat_a.h; j++) {
            res[i] += mat_a.val[j][i] * column[j];
        }
    }
}

// Dot product between a column @a and a line @b, stored into the matrix @mat.
void matrix_dcl(size_t nb_lin, float *a, size_t nb_col, float *b, Matrix *mat) {
    for (size_t i = 0; i < nb_lin; i++) {
        for (size_t j = 0; j < nb_col; j++) {
            mat->val[i][j] += a[i] * b[j];
        }
    }
}
