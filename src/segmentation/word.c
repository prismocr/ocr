#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <errno.h>
#include "segmentation/segmentation.h"
#include "utils/matrix.h"
#include "utils/vector.h"
#include "utils/linked_list.h"
#include "imgproc/image.h"
#include "imgproc/morphology.h"
#include "utils/error.h"
#include "utils/linked_list.h"

static size_t average_space(Vector hist);

int word_new(size_t x, size_t y, size_t w, size_t h, Word **word) {
    assert(*word == NULL);

    *word = (Word *) malloc(sizeof(Word));
    if (*word == NULL) {
        set_last_errorf("Failed to allocate memory for word: %s",
                        strerror(errno));
        return 1;
    }

    **word = (Word){.x = x,
                    .y = y,
                    .w = w,
                    .h = h,
                    .length = 0,
                    .letters = NULL,
                    .next = NULL};

    mll_new(&(*word)->images);

    return 0;
}

void word_free(Word **word) {
    assert(*word != NULL);

    mll_free(&(*word)->images);
    free((*word)->letters);

    free(*word);
    *word = NULL;
}

int word_segment(Matrix line, Word **words) {
    Matrix kernel;
    Vector hist;

    image_threshold_otsu(&line);
    image_invert_color(255.f, &line);

    kernel = structuring_element(3, 3);
    dilate(&line, kernel);
    matrix_free(&kernel);

    hist = image_histogram_x(line);

    float height_thresh = vector_average(hist) * 0.2f;
    for (size_t i = 0; i < hist.size; i++) {
        hist.val[i] = hist.val[i] > height_thresh ? hist.val[i] : 0.f;
    }

    size_t space_threshold = (size_t)(0.4f * average_space(hist));
    kernel
      = structuring_element(1, space_threshold + (space_threshold + 1) % 2);
    opening(&line, kernel);
    matrix_free(&kernel);

    vector_free(&hist);
    hist = image_histogram_x(line);

    height_thresh = vector_average(hist) * 0.2f;
    for (size_t i = 0; i < hist.size; i++) {
        hist.val[i] = hist.val[i] > height_thresh ? hist.val[i] : 0.f;
    }

    size_t left = 0;
    size_t prev_left = 0;
    size_t next_left = 0;
    Word *first_word = NULL;
    Word *prev_word = NULL;
    for (size_t i = 0; i < hist.size; i++) {
        if (hist.val[i] < 1.f) {
            size_t right = i;
            if (i > left + 3) {
                while (i < hist.size && hist.val[i] < 1.f) {
                    next_left = i++;
                }

                Word *current_word = NULL;
                if (word_new(prev_left == 0 ? 0 : prev_left + 1, 0,
                             (right + i) / 2 - prev_left, line.h,
                             &current_word)) {
                    return 1;
                }

                if (first_word == NULL) {
                    first_word = current_word;
                } else { // prev_word is not NULL
                    prev_word->next = current_word;
                }
                prev_word = current_word;

                prev_left = (right + i) / 2;
                left = next_left;
            } else {
                left = right;
            }
        }
    }
    *words = first_word;

    vector_free(&hist);

    return 0;
}

static size_t average_space(Vector hist) {
    size_t nb_spaces = 0;
    size_t total_space = 0;
    for (size_t j = 0, left = 0; j < hist.size; j++) {
        if (hist.val[j] != 0.f) {
            if (j >= left + 3) {
                if (nb_spaces >= 1) {
                    total_space += j - left;
                }
                nb_spaces++;
            }
            left = j;
        }
    }

    return nb_spaces == 1 ? 0 : total_space / (nb_spaces - 1);
}
