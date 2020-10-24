#include "segmentation.h"
#include "image.h"
#include "matrix.h"
#include "morphology.h"
#include "linked_list.h"

// TODO remove these headers
#include "bitmap.h"
#include <stdio.h>

struct word {
    size_t length;
    char *letters;
    Matrix *images;
};

static MatrixLinkedList get_word_images(Matrix image, Matrix hist);
static Matrix processed_histogram_y(Matrix image);
static Matrix histogram_y(Matrix image);
static Matrix histogram_x(Matrix image);
static float average_height(Matrix hist);
static void extract_words(Matrix *image);

/*
 * Text line segmentation based on morphology and histogram projection
 * http://www.cvc.uab.es/icdar2009/papers/3725a651.pdf
 */
void segment_morph_hist(Matrix image) {
    Matrix image_copy;
    matrix_copy(image, &image_copy);

    Matrix kernel = structuring_element(3, 3);
    smooth(&image_copy, kernel);
    matrix_free(&kernel);

    feature_extract_morph_based(&image_copy);

    Matrix hist_y = processed_histogram_y(image_copy);
    matrix_free(&image_copy);
    // TODO: Perform line recovery

    MatrixLinkedList word_images = get_word_images(image, hist_y);
    matrix_free(&hist_y);

    char buff[200];
    size_t c = 0;
    for (size_t i = 0; i < word_images.length; i++) {
        Matrix *word = mll_get(i, word_images);
        // TODO test other operations
        Matrix kernel = structuring_element(3, 3);
        dilate(word, kernel);
        matrix_free(&kernel);
        image_threshold_inv(120.f, 255.f, word);

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

        // TODO also do crop before

        Matrix cropped_word
          = image_crop(left, top, right - left, bot - top, *word);
        Matrix word_hist = histogram_x(cropped_word);

        left = 0;
        size_t j;
        for (j = 0; j < word_hist.h; j++) {
            if (word_hist.val[j][0] == 0.f) {
                if (j > left + 3) {
                    Matrix caracter = image_crop(left, 0, j - left - 2,
                                                 cropped_word.h, cropped_word);
                    sprintf(buff, "seg/car-%lu.bmp", c++);
                    bitmap_save(buff, &caracter);
                    matrix_free(&caracter);
                }
                left = j;
            }
        }
        if (j > left + 3) {
            Matrix caracter
              = image_crop(left, 0, j - left - 2, cropped_word.h, cropped_word);
            sprintf(buff, "seg/car-%lu.bmp", c++);
            bitmap_save(buff, &caracter);
            matrix_free(&caracter);
        }

        matrix_free(&word_hist);
        matrix_free(&cropped_word);
    }

    mll_free(&word_images);
}

/* LEO
 * Morphology based feature extraction method
 * Proposed by Wu, Hsieh and Chen:
 * https://www.cin.ufpe.br/~if751/projetos/artigos/Morphology-based%20text%20line%20extraction.pdf
 */
void feature_extract_morph_based(Matrix *image) {
    Matrix kernel, closed;

    matrix_free(&kernel);

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

void extract_words(Matrix *image) {
    Matrix kernel;

    // TODO test other operations
    kernel = structuring_element(1, 21);
    erode(image, kernel);
    erode(image, kernel);
    matrix_free(&kernel);

    image_threshold_inv(70.f, 255.f, image);
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

MatrixLinkedList get_word_images(Matrix image, Matrix hist) {
    MatrixLinkedList word_images;
    mll_new(&word_images);

    size_t top = 0;
    for (size_t i = 0; i < hist.h; i++) {
        if (hist.val[i][0] == 0.f) {
            if (i > top + 3) {
                Matrix line = image_crop(0, top, image.w, i - top - 2, image);
                extract_words(&line);
                Matrix line_hist = histogram_x(line);

                // Threshold based on average word height
                float height_thresh = matrix_average(line_hist) * 0.2f;
                for (size_t j = 0; j < line_hist.h; j++) {
                    line_hist.val[j][0] = line_hist.val[j][0] > height_thresh
                                            ? line_hist.val[j][0]
                                            : 0;
                }

                size_t left = 0;
                for (size_t j = 0; j < line_hist.h; j++) {
                    if (line_hist.val[j][0] == 0.f) {
                        if (j > left + 3) {
                            mll_insert(word_images.length,
                                       image_crop(left, top, j - left - 2,
                                                  i - top - 2, image),
                                       &word_images);
                        }
                        left = j;
                    }
                }

                matrix_free(&line_hist);
                matrix_free(&line);
            }
            top = i;
        }
    }

    return word_images;
}

Matrix processed_histogram_y(Matrix image) {
    Matrix hist_y = histogram_y(image);

    // Threshold based on average line length
    float length_thresh = matrix_average(hist_y) * 0.2f; // TODO: tweak value
    for (size_t i = 0; i < hist_y.h; i++) {
        hist_y.val[i][0]
          = hist_y.val[i][0] > length_thresh ? hist_y.val[i][0] : 0;
    }

    // Threshold based on line height
    float height_thresh = average_height(hist_y) * 0.2f; // TODO: tweak value
    size_t top = 0;
    for (size_t i = 0; i < hist_y.h; i++) {
        if (hist_y.val[i][0] == 0.f) {
            if (i - top < height_thresh + 2) {
                // Sets lines below height_thresh to 0
                for (; top < i; top++) {
                    hist_y.val[top][0] = 0.f;
                }
            }
            top = i;
        }
    }

    return hist_y;
}

static Matrix histogram_y(Matrix image) {
    Matrix hist;

    matrix_new(image.h, 1, &hist);
    for (size_t i = 0; i < image.h; i++) {
        for (size_t j = 0; j < image.w; j++) {
            hist.val[i][0] += image.val[i][j];
        }
    }

    return hist;
}

static Matrix histogram_x(Matrix image) {
    Matrix hist;

    matrix_new(image.w, 1, &hist);
    for (size_t i = 0; i < image.h; i++) {
        for (size_t j = 0; j < image.w; j++) {
            hist.val[j][0] += image.val[i][j];
        }
    }

    return hist;
}

float average_height(Matrix hist) {
    size_t nlines, sum_height, top;

    sum_height = 0;
    nlines = 0;
    top = 0;
    for (size_t i = 0; i < hist.h; i++) {
        if (hist.val[i][0] == 0.f) {
            if (i > top + 3) { // + 3 because line is at least 1px
                sum_height += i - top - 2;
                nlines++;
            }
            top = i;
        }
    }

    return (float) sum_height / nlines;
}
