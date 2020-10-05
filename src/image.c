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

void image_invert_color(float maxval, Matrix *image) {
    size_t i, j;

    for (i = 0; i < image->h; i++) {
        for (j = 0; j < image->w; j++) {
            image->val[i][j] = maxval - image->val[i][j];
        }
    }
}

Matrix image_crop(size_t x, size_t y, size_t w, size_t h, Matrix image) {
    assert(x + w <= image.w && y + h <= image.h);
    size_t i, j;
    Matrix cropped_image;

    matrix_new(h, w, &cropped_image);
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

float clamp(float value, float min, float max) {
    if (value <= min)
        return min;
    if (value >= max)
        return max;

    return value;
}

void image_contrast(Matrix *image, float delta) {
    size_t i, j;
    double factor = (259.f * (delta + 255.f)) / (255.f * (259.f - delta));

    for (i = 0; i < image->h; i++) {
        for (j = 0; j < image->w; j++) {
            float color = factor * (image->val[i][j] * 255.f - 128) + 128;
            image->val[i][j] = clamp(color, 0.f, 255.f) / 255.f;
        }
    }
}
