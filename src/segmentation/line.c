#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include <string.h>
#include "segmentation/segmentation.h"
#include "imgproc/image.h"
#include "imgproc/morphology.h"
#include "utils/matrix.h"
#include "utils/vector.h"
#include "utils/error.h"

static Vector processed_histogram_y(Matrix image);
static float average_height(Vector hist);

int line_new(size_t x, size_t y, size_t w, size_t h, Line **line) {
    assert(*line == NULL);

    *line = (Line *) malloc(sizeof(Line));
    if (*line == NULL) {
        set_last_errorf("Failed to allocate memory for line: %s",
                        strerror(errno));
        return 1;
    }

    (*line)->x = x;
    (*line)->y = y;
    (*line)->w = w;
    (*line)->h = h;
    (*line)->words = NULL;
    (*line)->next = NULL;

    return 0;
}

void line_free(Line **line) {
    assert(*line != NULL);

    Word *w = (*line)->words;
    Word *next = NULL;
    while (w != NULL) {
        next = w->next;
        word_free(&w);
        w = next;
    }
    (*line)->words = NULL;

    free(*line);
    *line = NULL;
}

/*
 * Morphology based feature extraction method
 * Proposed by Wu, Hsieh and Chen:
 * https://www.cin.ufpe.br/~if751/projetos/artigos/Morphology-based%20text%20line%20extraction.pdf
 */
static void feature_extract_morph_based(Matrix *image);

int line_segment_morph_hist(Matrix region, Line **lines) {
    // Matrix kernel = structuring_element(3, 3);
    // smooth(&image_copy, kernel);
    // matrix_free(&kernel);

    feature_extract_morph_based(&region);

    Vector hist = processed_histogram_y(region);

    // Perform line recovery
    size_t top = 0;
    size_t prev_top = 0;
    size_t next_top = 0;
    Line *first_line = NULL;
    Line *prev_line = NULL;
    for (size_t i = 0; i < hist.size; i++) {
        if (hist.val[i] == 0.f) {
            size_t bot = i;
            if (bot > top + 3) {
                while (i < hist.size && hist.val[i] == 0.f) {
                    next_top = i++;
                }

                Line *current_line = NULL;
                if (line_new(0, (top + prev_top) / 2 + 1, region.w,
                             (bot + i) / 2 - (top + prev_top) / 2 - 2,
                             &current_line)) {
                    return 1;
                }

                if (first_line == NULL) {
                    first_line = current_line;
                } else { // prev_line is not NULL
                    prev_line->next = current_line;
                }
                prev_line = current_line;

                prev_top = (bot + i) / 2;
                top = next_top;
            } else {
                top = bot;
            }
        }
    }
    *lines = first_line;

    vector_free(&hist);

    return 0;
}

static void feature_extract_morph_based(Matrix *image) {
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

/*
 * Morphological preprocessing for water flow segmentation
 */
static void morphological_preproc(Matrix *image);

int line_segment_water_flow(Matrix region) {
    Matrix image_copy;

    matrix_copy(region, &image_copy);
    morphological_preproc(&image_copy);

    // TODO: Implement end of the paper

    return 0;
}

static void morphological_preproc(Matrix *image) {
    Matrix kernel;

    // TODO: Apply otsu binarization before

    kernel = structuring_element(3, 3);
    erode(image, kernel);
    dilate(image, kernel);
    dilate(image, kernel);
    closing(image, kernel);

    matrix_free(&kernel);
}
