#include "segmentation/segmentation.h"
#include "imgproc/image.h"
#include "imgproc/morphology.h"
#include "utils/matrix.h"
#include "utils/vector.h"
#include <stdio.h>

// TODO remove these headers
//#include "utils/bitmap.h"
//#include <stdio.h>

// static size_t test_word = 0;

int character_segment(Matrix word, MatrixLinkedList *characters) {
    Matrix word_copy = {0};
    matrix_copy(word, &word_copy);

    // image_threshold_otsu(&word_copy);
    image_invert_color(255.f, &word_copy);

    /*
    Matrix kernel = structuring_element(5, 1);
    opening(&word_copy, kernel);
    matrix_free(&kernel);

    kernel = structuring_element(1, 3);
    closing(&word_copy, kernel);
    matrix_free(&kernel);
    */

    size_t top, bot;
    top = word_copy.h;
    bot = word_copy.h;
    for (size_t y = 0; y < word_copy.h; y++) {
        for (size_t x = 0; x < word_copy.w; x++) {
            if (top == word_copy.h && word_copy.val[y][x] > 0.f) {
                top = y;
            }
            if (bot == word_copy.h
                && word_copy.val[word_copy.h - y - 1][x] > 0.f) {
                bot = word_copy.h - y - 1;
            }
        }
    }

    // blank image
    if (top == bot) {
        return 0;
    }

    size_t left, right;
    left = word_copy.w;
    right = word_copy.w;
    for (size_t x = 0; x < word_copy.w; x++) {
        for (size_t y = 0; y < word_copy.h; y++) {
            if (left == word_copy.w && word_copy.val[y][x]) {
                left = x;
            }
            if (right == word_copy.w
                && word_copy.val[y][word_copy.w - x - 1] > 0.f) {
                right = word_copy.w - x - 1;
            }
        }
    }

    if (left == right) {
        return 0;
    }

    // TODO also do crop before
    Matrix cropped_word
      = image_crop(left, top, right - left, bot - top, word_copy);
    matrix_free(&word_copy);

    image_threshold_otsu(&cropped_word);
    Matrix kernel
      = structuring_element(cropped_word.h + (cropped_word.h + 1) % 2, 1);
    dilate(&cropped_word, kernel);
    matrix_free(&kernel);

    Vector word_hist = image_histogram_x(cropped_word);

    // TODO remove
    // char buff[200];
    // sprintf(buff, "seg/test-%zu.bmp", test_word++);
    // bitmap_save(buff, &cropped_word);

    size_t left_cropped_word = left;
    left = 0;
    size_t j;
    for (j = 0; j < word_hist.size; j++) {
        if (word_hist.val[j] < 1.f) {
            if (j >= left + 3) {
                Matrix character = image_crop(left_cropped_word + left, 0,
                                              j - left, cropped_word.h, word);
                Matrix processed_character = pre_process_char(&character);
                mll_insert(characters->length, processed_character, characters);
                matrix_free(&character);
            }
            left = j;
        }
    }
    if (j >= left + 3) {
        Matrix character = image_crop(left_cropped_word + left, 0, j - left,
                                      cropped_word.h, word);
        Matrix processed_character = pre_process_char(&character);
        for(size_t i=0; i<processed_character.h; i++){
            for(size_t j=0; j<processed_character.w; j++){
                float f = processed_character.val[i][j];
                if(f>0.1)
                    printf("\033[41m");
                printf("%.2f \033[00m",f);
            }
            printf("\n");
        }
        printf("\n");
        mll_insert(characters->length, processed_character, characters);
        matrix_free(&character);
    }

    vector_free(&word_hist);
    matrix_free(&cropped_word);

    return 0;
}

// int character_segment_test(Matrix word, MatrixLinkedList *characters) {
// TODO try conn recursive grass fire algorithm
// return 0;
//}
