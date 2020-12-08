#include "segmentation/segmentation.h"
#include "imgproc/image.h"
#include "imgproc/morphology.h"
#include "utils/matrix.h"
#include "utils/vector.h"

// TODO remove these headers
#include "utils/bitmap.h"
#include <stdio.h>

int character_segment(Matrix word, MatrixLinkedList *characters) {
    Matrix trimmed_word;
    if (image_trim(word, &trimmed_word)) {
        return 1;
    }

    Matrix trimmed_word_copy;
    matrix_copy(trimmed_word, &trimmed_word_copy);
    image_threshold_otsu(&trimmed_word);
    image_invert_color(255.f, &trimmed_word_copy);

    Matrix kernel
      = structuring_element(trimmed_word.h + (trimmed_word.h + 1) % 2, 1);
    dilate(&trimmed_word, kernel);
    matrix_free(&kernel);

    Vector hist = image_histogram_x(trimmed_word);

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

                Matrix character
                  = image_crop((left + prev_left) / 2, 0,
                               (right + i - (left + prev_left)) / 2 + 1,
                               trimmed_word.h, trimmed_word_copy);
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
    matrix_free(&trimmed_word_copy);
    matrix_free(&trimmed_word);

    return 0;
}
