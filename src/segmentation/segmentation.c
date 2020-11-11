#include "segmentation/segmentation.h"
#include "imgproc/image.h"
#include "utils/matrix.h"
#include "imgproc/morphology.h"
#include "utils/linked_list.h"

// TODO remove these headers
#include "utils/bitmap.h"
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

// TODO Do iterative form of the algo to avoid stack overflow
// I use pointer to matrix here to have lighter stackframes
// 4-conn recursive grass fire algorithm
void rec_grass_fire(size_t i, size_t j, Matrix *image, float class) {
    image->val[i][j] = class;

    // North
    if (i > 0 && image->val[i - 1][j] == 255.f) {
        rec_grass_fire(i - 1, j, image, class);
    }

    // South
    if (i < image->h - 1 && image->val[i + 1][j] == 255.f) {
        rec_grass_fire(i + 1, j, image, class);
    }

    // West
    if (j > 0 && image->val[i][j - 1] == 255.f) {
        rec_grass_fire(i, j - 1, image, class);
    }

    // East
    if (j < image->h - 1 && image->val[i][j + 1] == 255.f) {
        rec_grass_fire(i, j + 1, image, class);
    }
}

void segment_rlsa(Matrix image) {
    // TODO: pass image to don't apply otsu on every lines
    Matrix image_copy;
    matrix_copy(image, &image_copy);
    image_threshold_otsu(&image_copy);
    image_invert_color(255.f, &image_copy);

    Matrix horizontal_morph_im;
    matrix_copy(image_copy, &horizontal_morph_im);

    Matrix kernel = structuring_element(1, 101);
    dilate(&horizontal_morph_im, kernel);
    erode(&horizontal_morph_im, kernel);
    matrix_free(&kernel);

    Matrix vertical_morph_im;
    matrix_copy(image_copy, &vertical_morph_im);
    matrix_free(&image_copy);

    kernel = structuring_element(201, 1);
    dilate(&vertical_morph_im, kernel);
    erode(&vertical_morph_im, kernel);
    matrix_free(&kernel);

    for (size_t i = 0; i < image.h; i++) {
        for (size_t j = 0; j < image.w; j++) {
            horizontal_morph_im.val[i][j]
              = vertical_morph_im.val[i][j] * horizontal_morph_im.val[i][j]
                    >= 255.f
                  ? 255.f
                  : 0.f;
        }
    }

    matrix_free(&vertical_morph_im);

    kernel = structuring_element(1, 31);
    dilate(&horizontal_morph_im, kernel);
    matrix_free(&kernel);

    kernel = structuring_element(1, 11);
    erode(&horizontal_morph_im, kernel);
    matrix_free(&kernel);

    kernel = structuring_element(11, 1);
    dilate(&horizontal_morph_im, kernel);
    matrix_free(&kernel);

    float class = 256.f;
    for (size_t i = 0; i < horizontal_morph_im.h; i++) {
        for (size_t j = 0; j < horizontal_morph_im.w; j++) {
            if (horizontal_morph_im.val[i][j] == 255.f) {
                rec_grass_fire(i, j, &horizontal_morph_im, class ++);
            }
        }
    }

    // consider bounding box as roi
    MatrixLinkedList regions;
    mll_new(&regions);
    for (float c = 256.f; c < class; c++) {
        size_t top, bot, left, right;
        top = image.h;
        bot = 0;
        left = image.w;
        right = 0;

        for (size_t i = 0; i < horizontal_morph_im.h; i++) {
            for (size_t j = 0; j < horizontal_morph_im.w; j++) {
                if (horizontal_morph_im.val[i][j] == c) {
                    if (i < top) {
                        top = i;
                    }
                    if (i > bot) {
                        bot = i;
                    }
                    if (j < left) {
                        left = j;
                    }
                    if (j > right) {
                        right = j;
                    }
                }
            }
        }

        if (right > left && bot > top) {
            mll_insert(regions.length,
                       image_crop(left, top, right - left, bot - top, image),
                       &regions);
        }
    }
    matrix_free(&horizontal_morph_im);

    char buff[200];
    for (size_t i = 0; i < regions.length; i++) {
        sprintf(buff, "seg/region-%lu.bmp", i);
        bitmap_save(buff, mll_get(i, regions));
        segment_morph_hist(*mll_get(i, regions));
    }

    mll_free(&regions);
}

static size_t line_count = 0;
static size_t word_count = 0;
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

    Matrix hist_y = processed_histogram_y(image_copy);
    matrix_free(&image_copy);

    MatrixLinkedList word_images = get_word_images(image, hist_y);
    matrix_free(&hist_y);

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
        Matrix word_hist = histogram_x(cropped_word);

        sprintf(buff, "seg/word-%lu.bmp", i);
        // bitmap_save(buff, &cropped_word);

        size_t left_cropped_word = left;
        left = 0;
        size_t j;
        for (j = 0; j < word_hist.h; j++) {
            if (word_hist.val[j][0] == 0.f) {
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

        matrix_free(&word_hist);
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

MatrixLinkedList get_word_images(Matrix image, Matrix hist) {
    MatrixLinkedList word_images;
    mll_new(&word_images);

    // Perform line recovery
    size_t top = 0;
    size_t prev_top = 0;
    size_t next_top = 0;
    for (size_t i = 0; i < hist.h; i++) {
        if (hist.val[i][0] == 0.f) {
            size_t bot = i;
            if (bot > top + 3) {
                while (i < hist.h && hist.val[i][0] == 0.f) {
                    next_top = i++;
                }

                Matrix line
                  = image_crop(0, (top + prev_top) / 2 + 1, image.w,
                               (bot + i) / 2 - (top + prev_top) / 2 - 2, image);

                // TOREMOVE
                char buff[200];
                sprintf(buff, "seg/line-%lu.bmp", line_count++);
                bitmap_save(buff, &line);

                extract_words(&line);

                Matrix line_hist = histogram_x(line);

                // TODO: tweak value
                float height_thresh = matrix_average(line_hist) * 0.3f;
                for (size_t i = 0; i < line_hist.h; i++) {
                    line_hist.val[i][0] = line_hist.val[i][0] > height_thresh
                                            ? line_hist.val[i][0]
                                            : 0.f;
                }

                // Average space
                size_t nb_spaces = 0;
                size_t total_space = 0;
                for (size_t j = 0, left = 0; j < line_hist.h; j++) {
                    if (line_hist.val[j][0] != 0.f) {
                        if (j >= left + 3) {
                            nb_spaces++;
                            total_space += j - left;
                        }
                        left = j;
                    }
                }
                float average_space = (float) total_space / nb_spaces;

                Matrix kernel = structuring_element(
                  1,
                  (size_t) average_space - (((size_t) average_space + 1) % 2));
                dilate(&line, kernel);
                matrix_free(&kernel);

                kernel = structuring_element(1, 3);
                erode(&line, kernel);
                matrix_free(&kernel);

                // TODO optimize this garbage
                matrix_free(&line_hist);
                line_hist = histogram_x(line);
                height_thresh = matrix_average(line_hist) * 0.3f;
                for (size_t i = 0; i < line_hist.h; i++) {
                    line_hist.val[i][0] = line_hist.val[i][0] > height_thresh
                                            ? line_hist.val[i][0]
                                            : 0.f;
                }

                // Word recovery
                size_t left = 0;
                size_t prev_left = 0;
                size_t next_left = 0;
                for (size_t j = 0; j < line_hist.h; j++) {
                    if (line_hist.val[j][0] == 0.f) {
                        size_t right = j;
                        if (j > left + 3) {
                            while (j < line_hist.h
                                   && line_hist.val[j][0] == 0.f) {
                                next_left = j++;
                            }

                            mll_insert(
                              word_images.length,
                              image_crop(
                                (left + prev_left) / 2 + 1,
                                (top + prev_top) / 2 + 1,
                                (right + j) / 2 - (left + prev_left) / 2 - 2,
                                (bot + i) / 2 - (top + prev_top) / 2 - 2,
                                image),
                              &word_images);

                            sprintf(buff, "seg/word-%lu.bmp", word_count++);
                            bitmap_save(buff, mll_get(word_images.length - 1,
                                                      word_images));

                            prev_left = (right + j) / 2;
                            left = next_left;
                        } else {
                            left = right;
                        }
                    }
                }

                matrix_free(&line_hist);
                matrix_free(&line);
                prev_top = (bot + i) / 2;
                top = next_top;
            } else {
                top = bot;
            }
        }
    }

    return word_images;
}

void extract_words(Matrix *image) {
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

    return nlines == 0 ? 0 : (float) sum_height / nlines;
}
