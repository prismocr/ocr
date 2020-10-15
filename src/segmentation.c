#include "segmentation.h"
#include "image.h"
#include "matrix.h"
#include "morphology.h"

// TODO remove these headers
#include "bitmap.h"
#include <stdio.h>

Matrix histogram_y(Matrix image);
float average_height(Matrix hist);

/*
 * Text line segmentation based on morphology and histogram projection
 * http://www.cvc.uab.es/icdar2009/papers/3725a651.pdf
 */
void segment_morph_hist(Matrix image) {
    size_t i, top;
    Matrix image_copy;
    matrix_copy(image, &image_copy);
    feature_extract_morph_based(&image_copy);

    Matrix hist = histogram_y(image_copy);

    // thresh based on average line length
    float length_thresh = matrix_average(hist) * 0.2f; // TODO: tweak value
    for (i = 0; i < hist.h; i++) {
        hist.val[i][0] = hist.val[i][0] > length_thresh ? hist.val[i][0] : 0;
    }

    float height_thresh = average_height(hist) * 0.2f; // TODO: tweak value
    top = 0;
    // TODO try to make this cleaner
    for (i = 0; i < hist.h; i++) {
        if (hist.val[i][0] == 0.f) {
            if (i - top < height_thresh + 2) {
                for (; top < i; top++) {
                    hist.val[top][0] = 0.f;
                }
            }
            top = i;
        }
    }

    // Save lines found to file
    top = 0;
    int d = 0;
    for (i = 0; i < hist.h; i++) {
        if (hist.val[i][0] == 0.f) {
            if (i > top + 3) {
                Matrix c = image_crop(0, top, image.w, i - top - 2, image);
                char buff[200];
                sprintf(buff, "seg/line-%d.bmp", d++);
                bitmap_save(buff, &c);
                matrix_free(&c);
            }
            top = i;
        }
    }

    // Not finished
}

/*
 * Morphology based feature extraction method
 * Proposed by Wu, Hsieh and Chen:
 * https://www.cin.ufpe.br/~if751/projetos/artigos/Morphology-based%20text%20line%20extraction.pdf
 */
void feature_extract_morph_based(Matrix *image) {
    Matrix kernel, closed;

    kernel = structuring_element(3, 3);
    smooth(image, kernel);
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

Matrix histogram_y(Matrix image) {
    size_t i, j;
    Matrix hist;

    matrix_new(image.h, 1, &hist);
    for (i = 0; i < image.h; i++) {
        for (j = 0; j < image.w; j++) {
            hist.val[i][0] += image.val[i][j];
        }
    }

    return hist;
}

float average_height(Matrix hist) {
    size_t i, nlines, sum_height, top;

    sum_height = 0;
    nlines = 0;
    top = 0;
    for (i = 0; i < hist.h; i++) {
        if (hist.val[i][0] == 0.f) {
            if (i > top + 3) {
                sum_height += i - top - 2;
                nlines++;
            }
            top = i;
        }

    }

    return (float) sum_height / nlines;
}

