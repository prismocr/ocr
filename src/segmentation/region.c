#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include <string.h>
#include "segmentation/segmentation.h"
#include "utils/matrix.h"
#include "imgproc/image.h"
#include "imgproc/morphology.h"
#include "imgproc/connected_components.h"
#include "utils/linked_list.h"
#include "utils/union_find.h"
#include "utils/error.h"

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
    assert(*region != NULL);

    Line *l = (*region)->lines;
    Line *next = NULL;
    while (l != NULL) {
        next = l->next;
        line_free(&l);
        l = next;
    }
    (*region)->lines = NULL;

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

    // Perform AND between vertical and horizontal
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

    UnionFind u;
    cc_labeling(&horizontal_morph_im, &u);

    // consider bounding box as region of interest
    Region *first_region = NULL;
    Region *prev_region = NULL;
    int n = 0;
    while (n < u.num_nodes) {
        int root = uf_find(n, &u);
        if (root == -1) {
            n += 1;
            continue;
        }

        for (int i = n; i < u.num_nodes; i++) {
            if (uf_find(i, &u) == root) {
                u.parents[i] = -1;
            }
        }
        float c = 256 + root;

        size_t x, y, w, h;
        cc_bounding_box(horizontal_morph_im, c, &x, &y, &w, &h);

        if (w > 0 && h > 0) {
            Region *current_region = NULL;
            if (region_new(x, y, w, h, &current_region)) {
                return 1;
            }

            if (first_region == NULL) {
                first_region = current_region;
            } else { // prev_region is not NULL
                prev_region->next = current_region;
            }
            prev_region = current_region;
        }

        n += 1;
    }
    *regions = first_region;

    uf_free(&u);
    matrix_free(&horizontal_morph_im);

    return 0;
}
