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

int image_trim(Matrix image, Matrix *out) {
    Matrix image_copy = {0};
    matrix_copy(image, &image_copy);

    // image_threshold_otsu(&image_copy);
    image_invert_color(255.f, &image_copy);

    // Find vertical bounds of image image
    size_t top, bot;
    top = image_copy.h;
    bot = image_copy.h;
    for (size_t y = 0; y < image_copy.h; y++) {
        for (size_t x = 0; x < image_copy.w; x++) {
            if (top == image_copy.h && image_copy.val[y][x] > 0.f) {
                top = y;
            }
            if (bot == image_copy.h
                && image_copy.val[image_copy.h - y - 1][x] > 0.f) {
                bot = image_copy.h - y - 1;
            }
        }
    }

    // blank image
    if (top == bot) {
        return 1;
    }

    // Find horizontal bounds of image image
    size_t left, right;
    left = image_copy.w;
    right = image_copy.w;
    for (size_t x = 0; x < image_copy.w; x++) {
        for (size_t y = 0; y < image_copy.h; y++) {
            if (left == image_copy.w && image_copy.val[y][x]) {
                left = x;
            }
            if (right == image_copy.w
                && image_copy.val[y][image_copy.w - x - 1] > 0.f) {
                right = image_copy.w - x - 1;
            }
        }
    }

    if (left == right) {
        return 1;
    }

    *out = image_crop(left == 0 ? 0 : left - 1, top == 0 ? 0 : top - 1,
                      right == image.w ? right - left : right - left + 1,
                      bot == image.h ? bot - top : bot - top + 1, image_copy);

    matrix_free(&image_copy);

    return 0;
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
size_t check_white_row(Matrix *image, size_t row) {
    size_t w = image->w;

    for (size_t i = 0; i < w; i++)
        if (image->val[row][i] != 255.f)
            return 0;
    return 1;
}

size_t check_white_column(Matrix *image, size_t col) {
    size_t h = image->h;

    for (size_t i = 0; i < h; i++)
        if (image->val[i][col] != 255.f)
            return 0;
    return 1;
}

size_t image_bound_left(Matrix *image) {
    for (size_t x = 0; x < image->w; x++) {
        if (!check_white_column(image, x)) {
            return x;
        }
    }

    return 0;
}

size_t image_bound_right(Matrix *image) {
    for (int x = image->w - 1; x >= 0; x--) {
        if (!check_white_column(image, x)) {
            return x + 1;
        }
    }

    return image->w - 1;
}

size_t image_bound_top(Matrix *image) {
    for (size_t x = 0; x < image->h; x++) {
        if (!check_white_row(image, x)) {
            return x;
        }
    }

    return 0;
}

size_t image_bound_bottom(Matrix *image) {
    for (int x = image->h - 1; x >= 0; x--) {
        if (!check_white_row(image, x)) {
            return x + 1;
        }
    }

    return image->h - 1;
}

Matrix trim(Matrix *image) {
    size_t top = image_bound_top(image);
    size_t bot = image_bound_bottom(image);
    size_t left = image_bound_left(image);
    size_t right = image_bound_right(image);

    Matrix dest;
    matrix_new(abs((int) bot - (int) top), abs((int) right - (int) left),
               &dest);

    size_t x = 0, y = 0;
    for (size_t j = top; j < bot; j++) {
        for (size_t i = left; i < right; i++) {
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

float max_color(Matrix *image) {
    float m = 0;
    for (size_t x = 0; x < image->w; x++) {
        for (size_t y = 0; y < image->h; y++) {
            if (image->val[y][x] > m)
                m = image->val[y][x];
        }
    }

    return m;
}

void image_normalize_brightness(Matrix *image) {
    float m = max_color(image);
    matrix_scale(image, 0.99f * m / 255);
    matrix_add_const(image, 0.01f);
    /*for (size_t x = 0; x < image->w; x++) {
        for (size_t y = 0; y < image->h; y++) {
            image->val[y][x] *= (255.f / m);
        }
    }*/
}

Matrix pre_process_char(Matrix *image) {
    // Matrix cpy;
    // matrix_copy(*image, &cpy);
    // return cpy;
    // image_threshold_otsu(&image);
    Matrix img = trim(image);
    Matrix s = scale_square(&img, 28);

    image_levels(&s, 10);
    image_invert_color(255.f, &s);
    image_normalize_brightness(&s);

    matrix_free(&img);

    return s;
}

void image_levels(Matrix *mat, size_t n) {
    for (size_t x = 0; x < mat->w; x++) {
        for (size_t y = 0; y < mat->h; y++) {
            float v = mat->val[y][x];
            for (size_t i = 0; i < n; i++) {
                if (v >= i * (255.f / n) && v <= (i + 1) * (255.f / n)) {
                    mat->val[y][x] = (i + 1) * (255.f / n);
                }
            }
        }
    }
}
