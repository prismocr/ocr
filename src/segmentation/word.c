#include "segmentation/segmentation.h"
#include "utils/matrix.h"
#include "utils/vector.h"
#include "utils/linked_list.h"
#include "imgproc/image.h"
#include "imgproc/morphology.h"

// TODO remove these headers
#include "utils/bitmap.h"
#include <stdio.h>

static void extract_words(Matrix *image);

static size_t line_count = 0;
static size_t word_count = 0;

MatrixLinkedList segment_words(Matrix image, Vector hist) {
    MatrixLinkedList word_images;
    mll_new(&word_images);

    // Perform line recovery
    size_t top = 0;
    size_t prev_top = 0;
    size_t next_top = 0;
    for (size_t i = 0; i < hist.size; i++) {
        if (hist.val[i] == 0.f) {
            size_t bot = i;
            if (bot > top + 3) {
                while (i < hist.size && hist.val[i] == 0.f) {
                    next_top = i++;
                }

                Matrix line
                  = image_crop(0, (top + prev_top) / 2 + 1, image.w,
                               (bot + i) / 2 - (top + prev_top) / 2 - 2, image);

                // TOREMOVE
                char buff[200];
                sprintf(buff, "seg/line-%zu.bmp", line_count++);
                bitmap_save(buff, &line);

                extract_words(&line);

                Vector line_hist = image_histogram_x(line);

                // TODO: tweak value
                float height_thresh = vector_average(line_hist) * 0.3f;
                for (size_t i = 0; i < line_hist.size; i++) {
                    line_hist.val[i] = line_hist.val[i] > height_thresh
                                         ? line_hist.val[i]
                                         : 0.f;
                }

                // Average space
                size_t nb_spaces = 0;
                size_t total_space = 0;
                for (size_t j = 0, left = 0; j < line_hist.size; j++) {
                    if (line_hist.val[j] != 0.f) {
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
                vector_free(&line_hist);
                line_hist = image_histogram_x(line);
                height_thresh = vector_average(line_hist) * 0.3f;
                for (size_t i = 0; i < line_hist.size; i++) {
                    line_hist.val[i] = line_hist.val[i] > height_thresh
                                         ? line_hist.val[i]
                                         : 0.f;
                }

                // Word recovery
                size_t left = 0;
                size_t prev_left = 0;
                size_t next_left = 0;
                for (size_t j = 0; j < line_hist.size; j++) {
                    if (line_hist.val[j] == 0.f) {
                        size_t right = j;
                        if (j > left + 3) {
                            while (j < line_hist.size
                                   && line_hist.val[j] == 0.f) {
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

                            sprintf(buff, "seg/word-%zu.bmp", word_count++);
                            bitmap_save(buff, mll_get(word_images.length - 1,
                                                      word_images));

                            prev_left = (right + j) / 2;
                            left = next_left;
                        } else {
                            left = right;
                        }
                    }
                }

                vector_free(&line_hist);
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
