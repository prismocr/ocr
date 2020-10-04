#ifndef IMAGE_H
#define IMAGE_H

#include "matrix.h"

void image_threshold(float thresh, float maxval, Matrix *image);
void image_threshold_inv(float thresh, float maxval, Matrix *image);
void image_invert_color(Matrix *image);
void image_contrast(Matrix *image, float delta);

/*
 * Crops the image and return a copy
 */
Matrix image_crop(size_t x, size_t y, size_t w, size_t h, Matrix image);

void image_normalize(Matrix *image);

#endif // IMAGE_H
