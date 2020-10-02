#include "matrix.h"
#include <assert.h>
#include <stddef.h>

void image_threshold(float thresh, float maxval, Matrix *image) {
    size_t i, j;

    for (i = 0; i < image->h; i++) {
        for (j = 0; j < image->w; j++) {
            image->val[i][j] = image->val[i][j] > thresh ? maxval : 0.f;
        }
    }
}

void image_threshold_inv(float thresh, float maxval, Matrix *image) {
    size_t i, j;

    for (i = 0; i < image->h; i++) {
        for (j = 0; j < image->w; j++) {
            image->val[i][j] = image->val[i][j] > thresh ? 0.f : maxval;
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

Matrix image_crop(size_t x, size_t y, size_t w, size_t h, Matrix image) {
    size_t i, j;
    assert(x + w <= image.w && y + h <= image.h);

    Matrix cropped_image = matrix_new(h, w);

    for (i = 0; i < h; i++) {
        for (j = 0; j < w; j++) {
            cropped_image.val[i][j] = image.val[i + y][j + x];
        }
    }

    return cropped_image;
}

void image_normalize(Matrix *image) {
    size_t i, j;

    for (i = 0; i < image->h; i++) {
        for (j = 0; j < image->w; j++) {
            image->val[i][j] /= 255;
        }
    }
}
