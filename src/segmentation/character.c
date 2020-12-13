#include <stdlib.h>
#include <assert.h>
#include "segmentation/segmentation.h"
#include "imgproc/image.h"
#include "imgproc/morphology.h"
#include "imgproc/connected_components.h"
#include "utils/matrix.h"
#include "utils/vector.h"
#include "utils/utils.h"

// TODO: Remove
#include <stdio.h>

static int merge_dots(float height_thresh, float y_thresh,
                      ConnectedComponent *ccs, int num_cc);
static int merge_cc_with_following(size_t i, ConnectedComponent *ccs,
                                   int num_cc);

int character_segment(Matrix word, MatrixLinkedList *characters) {
    Matrix word_copy;
    matrix_copy(word, &word_copy);
    image_threshold_otsu(&word_copy);
    image_invert_color(255.f, &word_copy);

    Matrix kernel = structuring_element(word_copy.h + (word_copy.h + 1) % 2, 1);
    dilate(&word_copy, kernel);
    matrix_free(&kernel);

    Vector hist = image_histogram_x(word_copy);

    size_t left = 0;
    size_t prev_left = 0;
    size_t next_left = 0;
    for (size_t i = 0; i < hist.size; i++) {
        if (hist.val[i] < 1.f || i == hist.size - 1) {
            size_t right = i;
            if (i >= left + 3) {
                while (i < hist.size && hist.val[i] < 1.f) {
                    next_left = i++;
                }

                size_t w = i == hist.size ? i - 1 - prev_left : i - prev_left;
                Matrix character = image_crop(prev_left, 0, w, word.h, word);

                Matrix proc_character = pre_process_char(&character);
                mll_insert(characters->length, proc_character, characters);

                matrix_free(&proc_character);
                matrix_free(&character);

                prev_left = (right + i) / 2;
                left = next_left;
            } else {
                left = i;
            }
        }
    }

    vector_free(&hist);
    matrix_free(&word_copy);

    return 0;
}

int character_segment_cc(Matrix word, MatrixLinkedList *characters) {
    Matrix word_copy;
    matrix_copy(word, &word_copy);

    image_threshold_otsu(&word);
    image_invert_color(255.f, &word);

    UnionFind u;
    cc_labeling(&word, &u);

    ConnectedComponent *ccs
      = (ConnectedComponent *) calloc(u.num_nodes, sizeof(ConnectedComponent));
    int num_cc = 0;
    int max_cc = u.num_nodes;

    // Insertion sort connected components left to right
    for (int i = 0; i < u.num_nodes; i++) {
        int root = uf_find(i, &u);
        if (root == -1) {
            continue;
        }

        for (int j = i; j < u.num_nodes; j++) {
            if (uf_find(j, &u) == root) {
                u.parents[j] = -1;
            }
        }

        float class = 256.f + root;

        size_t x, y, w, h;
        cc_bounding_box(word, class, &x, &y, &w, &h);

        int j = num_cc++;
        while (j > 0 && ccs[j - 1].x > x) {
            ccs[j] = ccs[j - 1];
            j -= 1;
        }
        ccs[j] = (ConnectedComponent){
          .x = x,
          .y = y,
          .w = w,
          .h = h,
          .label = class,
        };
    }

    uf_free(&u);

    float avg_width = num_cc > 0 ? ccs[0].w : 0.f;
    float avg_height = num_cc > 0 ? ccs[0].h : 0.f;
    for (int i = 0; i < num_cc - 1; i++) {
        avg_width += ccs[i + 1].w;
        avg_height += ccs[i + 1].h;
    }
    avg_width /= num_cc > 0 ? num_cc : 1;
    avg_height /= num_cc > 0 ? num_cc : 1;

    // ccs below this thresh are considered as dots
    float height_thresh = avg_height * 0.25f;
    float y_thresh = word.h * 0.5f;
    num_cc = merge_dots(height_thresh, y_thresh, ccs, num_cc);

    float wh_factor_thresh = 1.6f;
    for (int i = 0; i < num_cc; i++) {
        if (ccs[i].w >= ccs[i].h * wh_factor_thresh) {
            // TODO: Check not more than max cc + realloc if needed
            if (num_cc < max_cc) {
                for (int j = num_cc + 1; j > i; j--) {
                    ccs[j] = ccs[j - 1];
                }
                num_cc += 1;

                ccs[i + 1] = (ConnectedComponent){
                  .x = ccs[i].x + ccs[i].w / 2,
                  .y = ccs[i].y,
                  .w = ccs[i].w / 2,
                  .h = ccs[i].h,
                  .label = ccs[i].label,
                };
                ccs[i].w /= 2;
            }
        }
    }

    for (int i = 0; i < num_cc; i++) {
        size_t x = 0;
        if (i > 0 && ccs[i - 1].x + ccs[i - 1].w + 1 < word.w) {
            x = min2(ccs[i - 1].x + ccs[i - 1].w + 1, ccs[i].x);
        }

        size_t w = word.w - x;
        if (i < num_cc - 1) {
            if (ccs[i + 1].x > x + 1) {
                w = max2(ccs[i + 1].x - 1 - x, ccs[i].w + ccs[i].x - x);
            } else {
                w = ccs[i].w + ccs[i].x - x;
            }
        }

        size_t y = 0;
        if (ccs[i].y >= 2) {
            y = ccs[i].y - 2;
        }

        size_t h = word.h - y;
        if (y + ccs[i].h + 4 < word.h) {
            h = ccs[i].h + 4;
        }
        Matrix character = image_crop(x, y, w, h, word_copy);

        Matrix proc_character = pre_process_char(&character);
        mll_insert(characters->length, proc_character, characters);

        matrix_free(&proc_character);
        matrix_free(&character);
    }

    free(ccs);
    matrix_free(&word_copy);

    return 0;
}

static int merge_dots(float height_thresh, float y_thresh,
                      ConnectedComponent *ccs, int num_cc) {
    for (int i = 0; i < num_cc; i++) {
        if (ccs[i].h <= height_thresh && ccs[i].y < y_thresh) {
            int dist_prev_cc
              = i > 0 ? ccs[i].x - (ccs[i - 1].x + ccs[i - 1].w) : 0;
            int dist_next_cc
              = i < num_cc - 1 ? ccs[i + 1].x - (ccs[i].x + ccs[i].w) : 0;

            if (i < num_cc - 1 && (i == 0 || (dist_next_cc <= dist_prev_cc))) {
                num_cc = merge_cc_with_following(i, ccs, num_cc);
            } else if (i > 0
                       && (i == num_cc - 1 || (dist_prev_cc < dist_next_cc))) {
                num_cc = merge_cc_with_following(i - 1, ccs, num_cc);
            }
        }
    }

    return num_cc;
}

static int merge_cc_with_following(size_t i, ConnectedComponent *ccs,
                                   int num_cc) {
    assert(i < (size_t)(num_cc - 1));

    size_t u = max2(ccs[i].x + ccs[i].w, ccs[i + 1].x + ccs[i + 1].w);
    size_t v = max2(ccs[i].y + ccs[i].h, ccs[i + 1].y + ccs[i + 1].h);
    ccs[i].x = min2(ccs[i].x, ccs[i + 1].x);
    ccs[i].y = min2(ccs[i].y, ccs[i + 1].y);
    ccs[i].w = u - ccs[i].x;
    ccs[i].h = v - ccs[i].y;

    for (int j = i + 1; j < num_cc - 1; j++) {
        ccs[j] = ccs[j + 1];
    }

    return num_cc - 1;
}
