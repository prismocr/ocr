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

void matrix_convolution(Matrix *mat, int mode) {//Matrix *mat, 
    Matrix convo_mat = matrix_new(3, 3);
    matrix_convolution_init(&convo_mat, mode);
    calculate_values_convolution(mat, &convo_mat); 
    matrix_free(&convo_mat);
}

void matrix_convolution_init(Matrix *mat, int mode) {
    int sharpen[3][3] = {{0, -1, 0},{-1, 5, -1},{0, -1, 0}};
    int identity[3][3] = {{0, 0, 0},{0, 1, 0},{0, 0, 0}};
    int edge_detection[3][3] = {{-1, -1, -1},{-1, 8, -1},{-1, -1, -1}};
    int index = 0;
    // TOFIX
    switch(mode) {
    case 0 :
    for (int i = 0; i < 9; i++) {
            mat->val[i%3][index] = identity[i%3][index];
            if ((i + 1) % 3 == 0){
                index++;
            }
        }
        break;
    case 1  :
        for (int i = 0; i < 9; i++) {
            mat->val[i%3][index] = sharpen[i%3][index];
            if ((i + 1) % 3 == 0){
                index++;
            }
        }
        break;
    case 2  :
        for (int i = 0; i < 9; i++) {
            mat->val[i%3][index] = edge_detection[i%3][index];
            if ((i + 1) % 3 == 0){
                index++;
            }
        }
        break;
    default :
        printf("matrix_convolution_init: mode out of range");
    }
}

void calculate_values_convolution(Matrix *mat, Matrix *convo_mat) {
    Matrix temp = matrix_new(mat->w, mat->h);

    for (size_t i = 0; i < mat->w; i++) {
        for (size_t j = 0; j < mat->h; j++) {
            float sum = 0;
            for (size_t k = 0; k < convo_mat->w; k++) {
                for (size_t l = 0; l < convo_mat->h; l++) {
                    if (i == 0 && k == 0) {
                        ++k;
                    }
                    if (j == 0 && j == 0) {
                        ++l;
                    }
                    if (i == mat->w-1 && k == 2){
                        continue;
                    }
                    if (j == mat->h-1 && l == 2){
                        continue;
                    }
                    sum += mat->val[i-1+k][j-1+l] * convo_mat->val[k][l];
                }
            }
            if (sum < 0) {
                sum = 0;
            }
            temp.val[i][j] = sum;
        }
    }
    for (size_t i = 0; i < mat->w; i++) {
        for (size_t j = 0; j < mat->h; j++) {
                mat->val[i][j] = temp.val[i][j];
        }
    }
    matrix_free(&temp);
}