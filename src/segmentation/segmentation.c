#include <stdlib.h>
#include "segmentation/segmentation.h"
#include "imgproc/image.h"
#include "utils/matrix.h"
#include "imgproc/morphology.h"
#include "utils/linked_list.h"

// TODO remove these headers
#include "utils/bitmap.h"
#include <stdio.h>

int segment(Matrix image, Page *page) {
    if (page_new(image.w, image.h, &page)) {
        return 1;
    }

    if (segment_regions_rlsa(image, &page->regions)) {
        return 1;
    }

    char buff[200];
    size_t i = 0;
    Region *region = page->regions;
    while (region != NULL) {
        Matrix region_image
          = image_crop(region->x, region->y, region->w, region->h, image);
        sprintf(buff, "seg/region-%zu.bmp", i);
        bitmap_save(buff, &region_image);

        segment_lines(region_image, &region->lines);

        size_t j = 0;
        Line *line = region->lines;
        while (line != NULL) {
            Matrix line_image
              = image_crop(region->x + line->x, region->y + line->y, line->w,
                           line->h, image);
            sprintf(buff, "seg/line-%zu-%zu.bmp", i, j);
            bitmap_save(buff, &line_image);

            segment_words(line_image, &line->words);

            size_t k = 0;
            Word *word = line->words;
            while (word != NULL) {
                Matrix word_image = image_crop(region->x + line->x + word->x,
                                               region->y + line->y + word->y,
                                               word->w, word->h, image);
                sprintf(buff, "seg/word-%zu-%zu-%zu.bmp", i, j, k);
                bitmap_save(buff, &word_image);

                word = word->next;
                k++;
                matrix_free(&word_image);
            }

            line = line->next;
            j++;
            matrix_free(&line_image);
        }

        region = region->next;
        i++;
        matrix_free(&region_image);
    }

    return 0;
}

void segment_morph_hist(Matrix image) {
    // MatrixLinkedList word_images = segment_words(image, hist_y);
    Line *lines = NULL;
    segment_lines(image, &lines);
    MatrixLinkedList word_images = {0};

    char buff[200];
    size_t c = 0;
    for (size_t i = 0; i < word_images.length; i++) {
        Matrix word_copy;
        matrix_copy(*mll_get(i, word_images), &word_copy);
        Matrix *word = &word_copy;
        image_threshold_otsu(word);
        image_invert_color(255.f, word);

        Matrix kernel = structuring_element(5, 1);
        opening(word, kernel);
        matrix_free(&kernel);

        /*
        kernel = structuring_element(1, 3);
        closing(word, kernel);
        matrix_free(&kernel);
        */

        size_t top, bot;
        top = word->h;
        bot = word->h;
        for (size_t y = 0; y < word->h; y++) {
            for (size_t x = 0; x < word->w; x++) {
                if (top == word->h && word->val[y][x] > 0.f) {
                    top = y;
                }
                if (bot == word->h && word->val[word->h - y - 1][x] > 0.f) {
                    bot = word->h - y - 1;
                }
            }
        }

        // blank image
        if (top == bot) {
            continue;
        }

        size_t left, right;
        left = word->w;
        right = word->w;
        for (size_t x = 0; x < word->w; x++) {
            for (size_t y = 0; y < word->h; y++) {
                if (left == word->w && word->val[y][x]) {
                    left = x;
                }
                if (right == word->w && word->val[y][word->w - x - 1] > 0.f) {
                    right = word->w - x - 1;
                }
            }
        }

        if (left == right) {
            continue;
        }

        // TODO also do crop before
        Matrix cropped_word
          = image_crop(left, top, right - left, bot - top, *word);
        Vector word_hist = image_histogram_x(cropped_word);

        sprintf(buff, "seg/word-%zu.bmp", i);
        // bitmap_save(buff, &cropped_word);

        size_t left_cropped_word = left;
        left = 0;
        size_t j;
        for (j = 0; j < word_hist.size; j++) {
            if (word_hist.val[j] == 0.f) {
                if (j >= left + 3) {
                    Matrix caracter
                      = image_crop(left_cropped_word + left, 0, j - left - 2,
                                   cropped_word.h, *mll_get(i, word_images));
                    sprintf(buff, "seg/car-%zu.bmp", c++);
                    bitmap_save(buff, &caracter);
                    matrix_free(&caracter);
                }
                left = j;
            }
        }
        if (j >= left + 3) {
            Matrix caracter
              = image_crop(left_cropped_word + left, 0, j - left - 2,
                           cropped_word.h, *mll_get(i, word_images));
            sprintf(buff, "seg/car-%zu.bmp", c++);
            bitmap_save(buff, &caracter);
            matrix_free(&caracter);
        }

        vector_free(&word_hist);
        matrix_free(&cropped_word);
    }

    mll_free(&word_images);
}

/*
 * Water flow algorithm for text line segmentation
 * https://www.researchgate.net/publication/216584186_A_New_Approach_to_Water_Flow_Algorithm_for_Text_Line_Segmentation
 */
void segment_water_flow(Matrix image) {
    Matrix image_copy;

    matrix_copy(image, &image_copy);
    morphological_preproc(&image_copy);

    // TODO: Implement end of the paper
}

/*
 * Morphological pre-processing
 * https://www.researchgate.net/publication/216584186_A_New_Approach_to_Water_Flow_Algorithm_for_Text_Line_Segmentation
 */
void morphological_preproc(Matrix *image) {
    Matrix kernel;

    // TODO: Apply otsu binarization before

    kernel = structuring_element(3, 3);
    erode(image, kernel);
    dilate(image, kernel);
    dilate(image, kernel);
    closing(image, kernel);

    matrix_free(&kernel);
}
