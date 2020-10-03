#include "convolution.h"
#include <stdio.h>
float calculate_values_convolution(size_t i, size_t j, Matrix *mat,
                                   Matrix *convo_mat);

void edge_detect(Matrix *image) {
    float convo_mat[3][3] = {{-1., -1., -1.}, {-1., 8., -1.}, {-1., -1., -1.}};

    Matrix mat = matrix_new(3, 3);
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            mat.val[i][j] = convo_mat[i][j];
        }
    }

    convolute(image, &mat);
}

void gauss(Matrix *image) {
    float convo_mat[3][3] = {{1 / 16.f, 2 / 16.f, 1 / 16.f},
                             {2 / 16.f, 4 / 16.f, 2 / 16.f},
                             {1 / 16.f, 2 / 16.f, 1 / 16.f}};

    Matrix mat = matrix_new(3, 3);
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            mat.val[i][j] = convo_mat[i][j];
        }
    }

    convolute(image, &mat);
}

void wide_gauss(Matrix *image) {
    float convo_mat[5][5]
      = {{2 / 159.f, 4 / 159.f, 5 / 159.f, 4 / 159.f, 2 / 159.f},
         {4 / 159.f, 9 / 159.f, 12 / 159.f, 9 / 159.f, 4 / 159.f},
         {5 / 159.f, 12 / 159.f, 15 / 159.f, 12 / 159.f, 5 / 159.f},
         {4 / 159.f, 9 / 159.f, 12 / 159.f, 9 / 159.f, 4 / 159.f},
         {2 / 159.f, 4 / 159.f, 5 / 159.f, 4 / 159.f, 2 / 159.f}};

    Matrix mat = matrix_new(5, 5);
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            mat.val[i][j] = convo_mat[i][j];
        }
    }

    convolute(image, &mat);
}

void sharpen(Matrix *image) {
    float convo_mat[3][3] = {{0., -1., 0.}, {-1., 5., -1.}, {0., -1., 0.}};
    Matrix mat = matrix_new(3, 3);
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            mat.val[i][j] = convo_mat[i][j];
        }
    }

    convolute(image, &mat);
}

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
