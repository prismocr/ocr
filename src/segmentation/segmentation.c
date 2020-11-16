#include <stdlib.h>
#include "segmentation/segmentation.h"
#include "imgproc/image.h"
#include "utils/matrix.h"
#include "utils/vector.h"
#include "imgproc/morphology.h"
#include "utils/linked_list.h"

// TODO remove these headers
#include "utils/bitmap.h"
#include <stdio.h>

static Vector processed_histogram_y(Matrix image);
static float average_height(Vector hist);

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
        Matrix region_image = image_crop(region->x, region->y, region->w, region->h, image);
        sprintf(buff, "seg/region-%lu.bmp", i);
        bitmap_save(buff, &region_image);
        segment_morph_hist(region_image);

        region = region->next;
        i++;
    }

    return 0;
}

/*
 * Text line segmentation based on morphology and histogram projection
 * http://www.cvc.uab.es/icdar2009/papers/3725a651.pdf
 */
void segment_morph_hist(Matrix image) {
    Matrix image_copy;
    matrix_copy(image, &image_copy);

    // Matrix kernel = structuring_element(3, 3);
    // smooth(&image_copy, kernel);
    // matrix_free(&kernel);

    feature_extract_morph_based(&image_copy);

    Vector hist_y = processed_histogram_y(image_copy);
    matrix_free(&image_copy);

    MatrixLinkedList word_images = segment_words(image, hist_y);
    vector_free(&hist_y);

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

        sprintf(buff, "seg/word-%lu.bmp", i);
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
                    sprintf(buff, "seg/car-%lu.bmp", c++);
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
            sprintf(buff, "seg/car-%lu.bmp", c++);
            bitmap_save(buff, &caracter);
            matrix_free(&caracter);
        }

        vector_free(&word_hist);
        matrix_free(&cropped_word);
    }

    mll_free(&word_images);
}

/*
 * Morphology based feature extraction method
 * Proposed by Wu, Hsieh and Chen:
 * https://www.cin.ufpe.br/~if751/projetos/artigos/Morphology-based%20text%20line%20extraction.pdf
 */
void feature_extract_morph_based(Matrix *image) {
    Matrix kernel, closed;

    kernel = structuring_element(1, 21);
    matrix_copy(*image, &closed);
    closing(&closed, kernel);
    opening(image, kernel);
    matrix_free(&kernel);

    difference(image, closed);
    matrix_free(&closed);

    kernel = structuring_element(5, 5);
    closing(image, kernel);
    matrix_free(&kernel);

    image_threshold(10.f, 255.f, image);
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

Vector processed_histogram_y(Matrix image) {
    Vector hist_y = image_histogram_y(image);

    // Threshold based on average line length
    float length_thresh = vector_average(hist_y) * 0.2f; // TODO: tweak value
    for (size_t i = 0; i < hist_y.size; i++) {
        hist_y.val[i] = hist_y.val[i] > length_thresh ? hist_y.val[i] : 0;
    }

    // Threshold based on line height
    float height_thresh = average_height(hist_y) * 0.2f; // TODO: tweak value
    size_t top = 0;
    for (size_t i = 0; i < hist_y.size; i++) {
        if (hist_y.val[i] == 0.f) {
            if (i - top < height_thresh + 2) {
                // Sets lines below height_thresh to 0
                for (; top < i; top++) {
                    hist_y.val[top] = 0.f;
                }
            }
            top = i;
        }
    }

    return hist_y;
}

float average_height(Vector hist) {
    size_t nlines, sum_height, top;

    sum_height = 0;
    nlines = 0;
    top = 0;
    for (size_t i = 0; i < hist.size; i++) {
        if (hist.val[i] == 0.f) {
            if (i > top + 3) { // + 3 because line is at least 1px
                sum_height += i - top - 2;
                nlines++;
            }
            top = i;
        }
    }

    return nlines == 0 ? 0 : (float) sum_height / nlines;
}
