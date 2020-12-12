#include "segmentation/segmentation.h"
#include "imgproc/image.h"
#include "imgproc/morphology.h"
#include "imgproc/connected_components.h"
#include "utils/matrix.h"
#include "utils/vector.h"
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
                Matrix character_copy;
                matrix_copy(character, &character_copy);
                image_threshold_otsu(&character_copy);
                image_invert_color(255.f, &character_copy);

                // Computes char height and width
                size_t c_left, c_top, c_w, c_h;
                cc_bounding_box(character_copy, 255.f, &c_left, &c_top, &c_w,
                                &c_h);
                float wh_factor_threshold = 1.5f;

                if (c_w < c_h * wh_factor_threshold) {
                    Matrix proc_character = pre_process_char(&character);
                    mll_insert(characters->length, proc_character, characters);
                    matrix_free(&proc_character);
                } else {
                    UnionFind u;
                    cc_labeling(&character_copy, &u);

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

                        size_t cc_left, cc_top, cc_w, cc_h;
                        cc_bounding_box(character_copy, c, &cc_left, &cc_top,
                                        &cc_w, &cc_h);

                        if (cc_w > 0 && cc_h > 0) {
                            Matrix split_character = image_crop(
                              cc_left, cc_top, cc_w, cc_h, character);

                            Matrix proc_character
                              = pre_process_char(&character);
                            mll_insert(characters->length, proc_character,
                                       characters);

                            matrix_free(&split_character);
                            matrix_free(&proc_character);
                        }

                        n += 1;
                    }
                    uf_free(&u);
                }

                matrix_free(&character_copy);
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
