#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <errno.h>
#include "imgproc/image.h"
#include "utils/matrix.h"
#include "imgproc/convolution.h"
#include "utils/error.h"

static inline float clamp(float value, float min, float max);
static int image_pixel_probabilities(Matrix image, float **pixel_probabilities);

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

int image_threshold_otsu(Matrix *image) {
    float *pixel_probs = NULL;
    return_on_error(image_pixel_probabilities(*image, &pixel_probs))

      float pixel_mean
      = 0.f;
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
            float intra_class_variance
              = c0_prob * c1_prob * class_mean_diff * class_mean_diff;
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

Vector image_histogram_x(Matrix image) {
    Vector hist;

    vector_new(image.w, &hist);
    for (size_t i = 0; i < image.h; i++) {
        for (size_t j = 0; j < image.w; j++) {
            hist.val[j] += image.val[i][j];
        }
    }

    return hist;
}

Vector image_histogram_y(Matrix image) {
    Vector hist;

    vector_new(image.h, &hist);
    for (size_t i = 0; i < image.h; i++) {
        for (size_t j = 0; j < image.w; j++) {
            hist.val[i] += image.val[i][j];
        }
    }

    return hist;
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

float anti_aliasing_point(Matrix *image, size_t x, size_t y) {
    if (x <= 0 || x >= image->h - 1 || y <= 0 || y >= image->w - 1)
        return 0;

    float a = clamp(image->val[x - 1][y], 0.f, 255.f);
    float b = clamp(image->val[x + 1][y], 0.f, 255.f);
    float c = clamp(image->val[x][y + 1], 0.f, 255.f);
    float d = clamp(image->val[x][y - 1], 0.f, 255.f);

    return (a + b + c + d) / 4;
}

static int image_pixel_probabilities(Matrix image,
                                     float **pixel_probabilities) {
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

    float nb_pixels_inv = 1.f / (image.h * image.w);
    for (int i = 0; i < 256; i++) {
        (*pixel_probabilities)[i] *= nb_pixels_inv;
    }

    return 0;
}
int check_white_row(Matrix *image, size_t row) {
    int h = image->h;

    for (int i = 0; i < h; i++)
        if (image->val[i][row] <= 240.f)
            return 0;
    return 1;
}

int check_white_column(Matrix *image, size_t col) {
    int w = image->w;

    for (int i = 0; i < w; i++)
        if (image->val[col][i] <= 240.f)
            return 0;
    return 1;
}

Matrix trim(Matrix *image) {
    int w = image->w;
    int h = image->h;

    size_t leftmost = 0;
    for (int row = 0; row < h; ++row) {
        if (check_white_row(image, row))
            leftmost = row;
        else
            break;
    }

    size_t rightmost = 0;
    for (int row = w - 1; row >= 0; --row) {
        if (check_white_row(image, row))
            rightmost = row;
        else
            break;
    }

    size_t topmost = 0;
    for (int col = 0; col < w; ++col) {
        if (check_white_column(image, col))
            topmost = col + 1;
        else
            break;
    }

    size_t bottommost = 0;
    for (int col = h - 1; col >= 0; --col) {
        if (check_white_column(image, col))
            bottommost = col;
        else
            break;
    }

    if (leftmost == rightmost) {
        leftmost = 0;
        rightmost = w;
    }

    if (bottommost == topmost) {
        topmost = 0;
        bottommost = h;
    }

    Matrix dest;
    matrix_new(abs((int) bottommost - (int) topmost),
               abs((int) rightmost - (int) leftmost), &dest);

    size_t x = 0, y = 0;
    for (size_t j = topmost; j < bottommost; j++) {
        for (size_t i = leftmost; i < rightmost; i++) {
            dest.val[x][y] = image->val[j][i];
            y++;
        }
        x++;
        y = 0;
    }

    return dest;
}

Matrix scale_stretch(Matrix *image, size_t w, size_t h) {
    Matrix dst;
    matrix_new(h, w, &dst);
    size_t width = image->w;
    size_t height = image->h;

    for (size_t x = 0; x < w; x++) {
        for (size_t y = 0; y < h; y++) {
            int srcX = (int) round((float) x / (float) w * (float) width);
            int srcY = (int) round((float) y / (float) h * (float) height);
            srcX = (int) fmin(srcX, width - 1);
            srcY = (int) fmin(srcY, height - 1);

            dst.val[y][x] = image->val[srcY][srcX];
        }
    }

    return dst;
}

Matrix scale_square(Matrix *image, size_t size) {
    float l = fmax(image->w, image->h);
    float aspect_ratio = (size - 2) / l;

    Matrix dst
      = scale_stretch(image, image->w * aspect_ratio, image->h * aspect_ratio);
    Matrix final;
    matrix_new(size, size, &final);

    for (size_t x = 0; x < size; x++) {
        for (size_t y = 0; y < size; y++) {
            final.val[y][x] = 255.f;
        }
    }

    size_t offset_x = (size - dst.w) / 2;
    size_t offset_y = (size - dst.h) / 2;
    for (size_t x = 0; x < dst.w; x++) {
        for (size_t y = 0; y < dst.h; y++) {
            final.val[y + offset_y][x + offset_x] = dst.val[y][x];
        }
    }

    matrix_free(&dst);
    return final;
}
