#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include <string.h>
#include "segmentation/segmentation.h"
#include "utils/matrix.h"
#include "imgproc/image.h"
#include "imgproc/morphology.h"
#include "utils/linked_list.h"
#include "utils/error.h"

static void rec_grass_fire(size_t i, size_t j, Matrix *image, float class);

int region_new(size_t x, size_t y, size_t w, size_t h, Region **region) {
    assert(*region == NULL);

    *region = (Region *) malloc(sizeof(Region));
    if (*region == NULL) {
        set_last_errorf("Failed to allocate memory for region: %s",
                        strerror(errno));
        return 1;
    }

    (*region)->x = x;
    (*region)->y = y;
    (*region)->w = w;
    (*region)->h = h;
    (*region)->lines = NULL;
    (*region)->next = NULL;

    return 0;
}

void region_free(Region **region) {
    // TODO: free lines

    free(*region);
    *region = NULL;
}

int region_segment_rlsa(Matrix page, Region **regions) {
    // TODO: pass image to don't apply otsu on every lines
    Matrix image_copy;
    matrix_copy(page, &image_copy);
    image_threshold_otsu(&image_copy);
    image_invert_color(255.f, &image_copy);

    Matrix horizontal_morph_im;
    matrix_copy(image_copy, &horizontal_morph_im);

    Matrix kernel = structuring_element(1, 101);
    dilate(&horizontal_morph_im, kernel);
    erode(&horizontal_morph_im, kernel);
    matrix_free(&kernel);

    Matrix vertical_morph_im;
    matrix_copy(image_copy, &vertical_morph_im);
    matrix_free(&image_copy);

    kernel = structuring_element(201, 1);
    dilate(&vertical_morph_im, kernel);
    erode(&vertical_morph_im, kernel);
    matrix_free(&kernel);

    for (size_t i = 0; i < page.h; i++) {
        for (size_t j = 0; j < page.w; j++) {
            horizontal_morph_im.val[i][j]
              = vertical_morph_im.val[i][j] * horizontal_morph_im.val[i][j]
                    >= 255.f
                  ? 255.f
                  : 0.f;
        }
    }

    matrix_free(&vertical_morph_im);

    kernel = structuring_element(1, 31);
    dilate(&horizontal_morph_im, kernel);
    matrix_free(&kernel);

    kernel = structuring_element(1, 11);
    erode(&horizontal_morph_im, kernel);
    matrix_free(&kernel);

    kernel = structuring_element(11, 1);
    dilate(&horizontal_morph_im, kernel);
    matrix_free(&kernel);

    float class = 256.f;
    for (size_t i = 0; i < horizontal_morph_im.h; i++) {
        for (size_t j = 0; j < horizontal_morph_im.w; j++) {
            if (horizontal_morph_im.val[i][j] == 255.f) {
                rec_grass_fire(i, j, &horizontal_morph_im, class ++);
            }
        }
    }

    // consider bounding box as region of interest
    Region *first_region = NULL;
    Region *prev_region = NULL;
    for (float c = 256.f; c < class; c++) {
        size_t top, bot, left, right;
        top = page.h;
        bot = 0;
        left = page.w;
        right = 0;

        for (size_t i = 0; i < horizontal_morph_im.h; i++) {
            for (size_t j = 0; j < horizontal_morph_im.w; j++) {
                if (horizontal_morph_im.val[i][j] == c) {
                    if (i < top) {
                        top = i;
                    }
                    if (i > bot) {
                        bot = i;
                    }
                    if (j < left) {
                        left = j;
                    }
                    if (j > right) {
                        right = j;
                    }
                }
            }
        }

        if (right > left && bot > top) {
            Region *current_region = NULL;
            if (region_new(left, top, right - left, bot - top,
                           &current_region)) {
                return 1;
            }

            if (first_region == NULL) {
                first_region = current_region;
            } else { // prev_region is not NULL
                prev_region->next = current_region;
            }
            prev_region = current_region;
        }
    }
    *regions = first_region;

    matrix_free(&horizontal_morph_im);

    return 0;
}

// TODO Do iterative form of the algo to avoid stack overflow
// I use pointer to matrix here to have lighter stackframes
// 4-conn recursive grass fire algorithm
static void rec_grass_fire(size_t i, size_t j, Matrix *image, float class) {
    image->val[i][j] = class;

    // North
    if (i > 0 && image->val[i - 1][j] == 255.f) {
        rec_grass_fire(i - 1, j, image, class);
    }

    // South
    if (i < image->h - 1 && image->val[i + 1][j] == 255.f) {
        rec_grass_fire(i + 1, j, image, class);
    }

    // West
    if (j > 0 && image->val[i][j - 1] == 255.f) {
        rec_grass_fire(i, j - 1, image, class);
    }

    // East
    if (j < image->h - 1 && image->val[i][j + 1] == 255.f) {
        rec_grass_fire(i, j + 1, image, class);
    }
}
