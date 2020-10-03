#include "convolution.h"
#include "matrix.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

void image_threshold(float thresh, float maxval, Matrix *image) {
    size_t i, j;

    for (i = 0; i < image->h; i++) {
        for (j = 0; j < image->w; j++) {
            image->val[i][j] = image->val[i][j] < thresh ? 0 : maxval;
        }
    }
}

void image_invert_color(Matrix *image) {
    size_t i, j;

    for (i = 0; i < image->h; i++) {
        for (j = 0; j < image->w; j++) {
            image->val[i][j] = 1 - image->val[i][j];
        }
    }
}

float clamp(float value) {
    if(value > 255) return 255;
    if(value < 0) return 0;

    return value;
}

void image_contrast(Matrix *image, float delta) {
    size_t i, j;
    double factor = (259.f * (delta + 255.f)) / (255.f * (259.f - delta));

    for (i = 0; i < image->h; i++) {
        for (j = 0; j < image->w; j++) {
            float color = factor * (image->val[i][j] * 255.f - 128) + 128;
            image->val[i][j] = clamp(color) / 255.f;
        }
    }
} 

void image_rotate(Matrix *image, float angle) {
    size_t i, j;
    
    size_t w = image->w;
    size_t h = image->h;

    float midx = w/2;
    float midy = h/2;

    Matrix dest = matrix_new(h, w);

    for(j = 0; j < h; j++) {
        for(i = 0; i < w; i++) {
            size_t x = (i - midx) * cos(angle) + (j - midy) * sin(angle) + midx;
            size_t y = (j - midx) * cos(angle) + (midx - i) * sin(angle) + midy;
            
            if(x < w && y < h) {
                dest.val[x][y] = image->val[i][j];
            }
        }
    }

    *image = dest;
}

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