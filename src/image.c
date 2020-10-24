#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <errno.h>
#include "image.h"
#include "matrix.h"
#include "convolution.h"
#include "error.h"

static inline float clamp(float value, float min, float max);

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

// TODO move that
int image_pixel_probabilities(Matrix image, float **pixel_probabilities) {
    *pixel_probabilities = (float *) calloc(256, sizeof(float));
    if (*pixel_probabilities == NULL) {
        set_last_errorf(
          "Failed to allocated memory for pixel probabilities: %s",
          strerror(errno));
        return 1;
    }

    for (size_t i = 0; i < image.h; i++) {
        for (size_t j = 0; j < image.w; j++) {
            (*pixel_probabilities)[(int) clamp(image.val[i][j], 0.f, 255.f)]++;
        }
    }

    float nb_pixels = image.h * image.w;
    for (int i = 0; i < 256; i++) {
        (*pixel_probabilities)[i] /= nb_pixels;
    }

    return 0;
}

int image_threshold_otsu(Matrix *image) {
    float *pixel_probs = NULL;
    return_on_error(image_pixel_probabilities(*image, &pixel_probs))

    float pixel_mean = 0.f;
    for (int i = 0; i < 256; i++) {
        pixel_mean += i * pixel_probs[i];
    }

    float c0_prob = pixel_probs[0];
    float c0_esp = 0.f;
    float max_intra_class_variance = 0.f;
    float best_thresh = 0;
    for (size_t t = 0; t < 256; t++) {
        float c1_prob = 1.f - c0_prob;
        if (c0_prob > 0 && c1_prob > 0) {
            float c0_mean = c0_esp / c0_prob;
            float c1_mean = (pixel_mean - c0_esp) / c1_prob;

            float class_mean_diff = c0_mean - c1_mean;
            float intra_class_variance = c0_prob * c1_prob * class_mean_diff * class_mean_diff;
            if (intra_class_variance > max_intra_class_variance) {
                max_intra_class_variance = intra_class_variance;
                best_thresh = t;
            }
        }

        c0_prob += pixel_probs[t];
        c0_esp += t * pixel_probs[t];
    }

    image_threshold(best_thresh, 255.f, image);

    free(pixel_probs);
    return 0;
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

void image_contrast(Matrix *image, float delta) {
    size_t i, j;
    double factor = (259.f * (delta + 255.f)) / (255.f * (259.f - delta));

    for (i = 0; i < image->h; i++) {
        for (j = 0; j < image->w; j++) {
            float color = factor * (image->val[i][j] * 255.f - 128) + 128;
            image->val[i][j] = clamp(color / 255.f, 0.f, 255.f);
        }
    }
}

void image_rotate(Matrix *image, float angle) {
    size_t i, j;

    size_t w = image->w;
    size_t h = image->h;

    float midx = w / 2;
    float midy = h / 2;

    Matrix dest;
    matrix_new(h, w, &dest);

    for (j = 0; j < h; j++) {
        for (i = 0; i < w; i++) {
            size_t x = (i - midx) * cos(angle) + (j - midy) * sin(angle) + midx;
            size_t y = (j - midx) * cos(angle) + (midx - i) * sin(angle) + midy;

            if (x < w && y < h) {
                dest.val[x][y] = image->val[i][j];
            }
        }
    }

    *image = dest;
}

void edge_detect(Matrix *image) {
    float convo_mat[3][3] = {{-1., -1., -1.}, {-1., 8., -1.}, {-1., -1., -1.}};

    Matrix mat;
    matrix_new(3, 3, &mat);

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

    Matrix mat;
    matrix_new(3, 3, &mat);

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
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

    Matrix mat;
    matrix_new(5, 5, &mat);

    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            mat.val[i][j] = convo_mat[i][j];
        }
    }

    convolute(image, &mat);
}

void sharpen(Matrix *image) {
    float convo_mat[3][3] = {{0., -1., 0.}, {-1., 5., -1.}, {0., -1., 0.}};

    Matrix mat;
    matrix_new(3, 3, &mat);

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            mat.val[i][j] = convo_mat[i][j];
        }
    }

    convolute(image, &mat);
}

static inline float clamp(float value, float min, float max) {
    if (value <= min)
        return min;
    if (value >= max)
        return max;

    return value;
}
