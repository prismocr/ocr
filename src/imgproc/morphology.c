#include "imgproc/morphology.h"
#include "utils/matrix.h"
#include <assert.h>
#include <float.h>

static inline float smooth_pixel(size_t y, size_t x, Matrix *image,
                                 Matrix kernel);
static inline float dilate_pixel(size_t y, size_t x, Matrix *image,
                                 Matrix kernel);
static inline float erode_pixel(size_t y, size_t x, Matrix *image,
                                Matrix kernel);

Matrix structuring_element(size_t m, size_t n) {
    assert(m > 0 && n > 0 && m % 2 == 1 && n % 2 == 1);
    size_t i, j;
    Matrix se;

    matrix_new(m, n, &se);
    for (i = 0; i < m; i++) {
        for (j = 0; j < n; j++) {
            se.val[i][j] = 1.f;
        }
    }

    return se;
}

int smooth(Matrix *image, Matrix kernel) {
    assert(kernel.h > 0 && kernel.w > 0 && kernel.h % 2 == 1
           && kernel.w % 2 == 1);
    size_t i, j;
    Matrix image_copy;

    if (matrix_copy(*image, &image_copy)) {
        return 1;
    }

    for (i = 0; i < image->h; i++) {
        for (j = 0; j < image->w; j++) {
            image->val[i][j] = smooth_pixel(i, j, &image_copy, kernel);
        }
    }

    matrix_free(&image_copy);

    return 0;
}

static inline float smooth_pixel(size_t y, size_t x, Matrix *image,
                                 Matrix kernel) {
    size_t i, j;
    float pixel;

    pixel = 0;
    for (i = 0; i < kernel.h; i++) {
        for (j = 0; j < kernel.w; j++) {
            if (y + i >= kernel.h / 2 && x + j >= kernel.w / 2
                && y + i < image->h + kernel.h / 2
                && x + j < image->w + kernel.w / 2) {
                pixel += image->val[y + i - kernel.h / 2][x + j - kernel.w / 2]
                         * kernel.val[i][j];
            }
        }
    }

    return pixel / (kernel.h * kernel.w);
}

int dilate(Matrix *image, Matrix kernel) {
    assert(kernel.h > 0 && kernel.w > 0 && kernel.h % 2 == 1
           && kernel.w % 2 == 1);
    size_t i, j;
    Matrix image_copy;

    if (matrix_copy(*image, &image_copy)) {
        return 1;
    }

    for (i = 0; i < image->h; i++) {
        for (j = 0; j < image->w; j++) {
            image->val[i][j] = dilate_pixel(i, j, &image_copy, kernel);
        }
    }

    matrix_free(&image_copy);

    return 0;
}

static inline float dilate_pixel(size_t y, size_t x, Matrix *image,
                                 Matrix kernel) {
    size_t i, j;
    float max, v;

    max = FLT_MIN;
    for (i = 0; i < kernel.h; i++) {
        for (j = 0; j < kernel.w; j++) {
            if (y + i >= kernel.h / 2 && x + j >= kernel.w / 2
                && y + i < image->h + kernel.h / 2
                && x + j < image->w + kernel.w / 2) {
                v = image->val[y + i - kernel.h / 2][x + j - kernel.w / 2]
                    * kernel.val[i][j];
                if (v > max) {
                    max = v;
                }
            }
        }
    }

    return max;
}

int erode(Matrix *image, Matrix kernel) {
    assert(kernel.h > 0 && kernel.w > 0 && kernel.h % 2 == 1
           && kernel.w % 2 == 1);
    size_t i, j;
    Matrix image_copy;

    if (matrix_copy(*image, &image_copy)) {
        return 1;
    }

    for (i = 0; i < image->h; i++) {
        for (j = 0; j < image->w; j++) {
            image->val[i][j] = erode_pixel(i, j, &image_copy, kernel);
        }
    }

    matrix_free(&image_copy);

    return 0;
}

static inline float erode_pixel(size_t y, size_t x, Matrix *image,
                                Matrix kernel) {
    size_t i, j;
    float min, v;

    min = FLT_MAX;
    for (i = 0; i < kernel.h; i++) {
        for (j = 0; j < kernel.w; j++) {
            if (y + i >= kernel.h / 2 && x + j >= kernel.w / 2
                && y + i < image->h + kernel.h / 2
                && x + j < image->w + kernel.w / 2) {
                v = image->val[y + i - kernel.h / 2][x + j - kernel.w / 2]
                    * kernel.val[i][j];
                if (v < min) {
                    min = v;
                }
            }
        }
    }

    return min;
}

void opening(Matrix *image, Matrix kernel) {
    assert(kernel.h > 0 && kernel.w > 0 && kernel.h % 2 == 1
           && kernel.w % 2 == 1);
    dilate(image, kernel);
    erode(image, kernel);
}

void closing(Matrix *image, Matrix kernel) {
    assert(kernel.h > 0 && kernel.w > 0 && kernel.h % 2 == 1
           && kernel.w % 2 == 1);
    erode(image, kernel);
    dilate(image, kernel);
}

void difference(Matrix *image1, Matrix image2) {
    assert(image1->h == image2.h && image1->w == image2.w);
    size_t i, j;
    float d;

    for (i = 0; i < image1->h; i++) {
        for (j = 0; j < image1->w; j++) {
            d = image1->val[i][j] - image2.val[i][j];
            image1->val[i][j] = d < 0 ? -d : d;
        }
    }
}
