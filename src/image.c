#include "matrix.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

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

float Restrict(float value) {
    if (value > 255)
        return 255;
    if (value < 0)
        return 0;

    return value;
}

void image_contrast(Matrix *image, float delta) {
    size_t i, j;
    double factor = (259.f * (delta + 255.f)) / (255.f * (259.f - delta));

    for (i = 0; i < image->h; i++) {
        for (j = 0; j < image->w; j++) {
            float color = factor * (image->val[i][j] * 255.f - 128) + 128;
            image->val[i][j] = Restrict(color) / 255.f;
        }
    }
}
