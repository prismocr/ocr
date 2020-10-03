#include "convolution.h"
#include <stdio.h>
float calculate_values_convolution(size_t i, size_t j, Matrix *mat,
                                   Matrix *convo_mat);

void convolute(Matrix *mat, Matrix *convo_mat) {
    Matrix temp = matrix_new(mat->h, mat->w);
    for (size_t i = 0; i < mat->h; i++) {
        for (size_t j = 0; j < mat->w; j++) {
            temp.val[i][j] = calculate_values_convolution(i, j, mat, convo_mat);
        }
    }

    matrix_free(mat);
    *mat = temp;

    matrix_free(convo_mat);
}

float calculate_values_convolution(size_t i, size_t j, Matrix *mat,
                                   Matrix *convo_mat) {
    float sum = 0;
    for (size_t k = 0; k < convo_mat->h; k++) {
        for (size_t l = 0; l < convo_mat->w; l++) {
            if (i + k >= convo_mat->h / 2 && j + l >= convo_mat->w / 2
                && i + k < mat->h + convo_mat->h / 2
                && j + l < mat->w + convo_mat->w / 2) {
                sum
                  += mat
                       ->val[i + k - convo_mat->h / 2][j + l - convo_mat->h / 2]
                     * convo_mat->val[k][l];
            }
        }
    }
    return sum < 0 ? 0 : sum;
}
