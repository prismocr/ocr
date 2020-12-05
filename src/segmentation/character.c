#include "segmentation/segmentation.h"
#include "imgproc/image.h"
#include "imgproc/morphology.h"
#include "utils/matrix.h"
#include "utils/vector.h"
#include <stdio.h>

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
    image_invert_color(255.f, &trimmed_word_copy);

    image_threshold_otsu(&trimmed_word);
    Matrix kernel
      = structuring_element(trimmed_word.h + (trimmed_word.h + 1) % 2, 1);
    dilate(&trimmed_word, kernel);
    matrix_free(&kernel);

    Vector word_hist = image_histogram_x(trimmed_word);

    size_t left = 0;
    size_t j;
    for (j = 0; j < word_hist.size; j++) {
        if (word_hist.val[j] < 1.f || j == word_hist.size - 1) {
            if (j >= left + 3) {
                Matrix character = image_crop(left, 0, j - left, trimmed_word.h,
                                              trimmed_word_copy);
                /*for(size_t i=0; i<character.h; i++){
                    for(size_t j=0; j<character.w; j++){
                        float f = character.val[i][j];
                        if(f>0)
                            printf("\033[00m");
                        printf("<>\033[41m");
                    }
                    printf("\033[00m\n");
                }
                printf("\n");*/
                Matrix proc_character = pre_process_char(&character);
                /*for(size_t i=0; i<proc_character.h; i++){
                    for(size_t j=0; j<proc_character.w; j++){
                        float f = proc_character.val[i][j];
                        if(f>0)
                            printf("\033[41m");
                        printf("<>\033[00m",f);
                    }
                    printf("\n");
                }
                printf("\n");*/
                mll_insert(characters->length, proc_character, characters);

                matrix_free(&proc_character);
                matrix_free(&character);
            }
            left = j;
        }
    }

    vector_free(&word_hist);
    matrix_free(&trimmed_word_copy);
    matrix_free(&trimmed_word);

    return 0;
}
