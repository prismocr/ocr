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

static void extract_words(Matrix *image);

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

    free(*word);
    *word = NULL;
}

int word_segment(Matrix line, Word **words) {
    // TODO: rename this function
    extract_words(&line);

    Vector hist = image_histogram_x(line);

    // TODO: tweak value
    float height_thresh = vector_average(hist) * 0.3f;
    for (size_t j = 0; j < hist.size; j++) {
        hist.val[j] = hist.val[j] > height_thresh ? hist.val[j] : 0.f;
    }

    // Average space
    size_t nb_spaces = 0;
    size_t total_space = 0;
    for (size_t j = 0, left = 0; j < hist.size; j++) {
        if (hist.val[j] != 0.f) {
            if (j >= left + 3) {
                nb_spaces++;
                total_space += j - left;
            }
            left = j;
        }
    }
    float average_space = (float) total_space / nb_spaces;

    Matrix kernel = structuring_element(
      1, (size_t) average_space - (((size_t) average_space + 1) % 2));
    dilate(&line, kernel);
    matrix_free(&kernel);

    kernel = structuring_element(1, 3);
    erode(&line, kernel);
    matrix_free(&kernel);

    // TODO optimize this garbage
    vector_free(&hist);
    hist = image_histogram_x(line);
    height_thresh = vector_average(hist) * 0.3f;
    for (size_t j = 0; j < hist.size; j++) {
        hist.val[j] = hist.val[j] > height_thresh ? hist.val[j] : 0.f;
    }

    // Word recovery
    size_t left = 0;
    size_t prev_left = 0;
    size_t next_left = 0;
    Word *first_word = NULL;
    Word *prev_word = NULL;
    for (size_t j = 0; j < hist.size; j++) {
        if (hist.val[j] == 0.f) {
            size_t right = j;
            if (j > left + 3) {
                while (j < hist.size && hist.val[j] == 0.f) {
                    next_left = j++;
                }

                Word *current_word = NULL;
                if (word_new((left + prev_left) / 2 + 1, 0,
                             (right + j) / 2 - (left + prev_left) / 2 - 2,
                             line.h, &current_word)) {
                    return 1;
                }

                if (first_word == NULL) {
                    first_word = current_word;
                } else { // prev_word is not NULL
                    prev_word->next = current_word;
                }
                prev_word = current_word;

                prev_left = (right + j) / 2;
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

static void extract_words(Matrix *image) {
    Matrix kernel;

    image_threshold_otsu(image);
    image_invert_color(255.f, image);

    kernel = structuring_element(image->h - (image->h + 1) % 2, 1);

    dilate(image, kernel);

    matrix_free(&kernel);

    /*
    kernel = structuring_element(1, 5);
    opening(image, kernel);
    matrix_free(&kernel);

    // eleminate short horizontal lines
    kernel = structuring_element(5, 1);
    closing(image, kernel);
    closing(image, kernel);
    matrix_free(&kernel);
    */
}
