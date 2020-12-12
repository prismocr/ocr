#include <stdlib.h>
#include "segmentation/segmentation.h"
#include "imgproc/image.h"
#include "imgproc/morphology.h"
#include "imgproc/connected_components.h"
#include "utils/matrix.h"
#include "utils/vector.h"

// TODO: Remove
#include <stdio.h>

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

    // TODO: merge dot of i
    // TODO: Check touching characters

    // ccs below this thresh are considered as dots
    float height_thresh = avg_height * 0.2f;
    float y_thresh = word.h * 0.5f;
    for (int i = 0; i < num_cc; i++) {
        if (ccs[i].h <= height_thresh && ccs[i].y < y_thresh) {
            printf("A DOT %zu %zu\n", ccs[i].y, ccs[i].h);
        }
    }

    for (int i = 0; i < num_cc; i++) {
        Matrix character
          = image_crop(ccs[i].x, ccs[i].y, ccs[i].w, ccs[i].h, word_copy);

        Matrix proc_character = pre_process_char(&character);
        mll_insert(characters->length, proc_character, characters);

        matrix_free(&proc_character);
        matrix_free(&character);
    }

    free(ccs);
    matrix_free(&word_copy);

    return 0;
}
