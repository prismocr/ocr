#ifndef IMAGE_H
#define IMAGE_H

#include "matrix.h"

// Apply treshold to an image
void image_threshold(float thresh, float maxval, Matrix *image);

// Apply treshold to an inverted image
void image_threshold_inv(float thresh, float maxval, Matrix *image);

// Invert images colors
void image_invert_color(Matrix *image);

// Crop image
Matrix image_crop(size_t x, size_t y, size_t w, size_t h, Matrix image);

// Change contrast level of an image
void image_contrast(Matrix *image, float delta);

// Rotate image by an arbitrary radian angle
void image_rotate(Matrix *image, float angle);

/**
 * Sharpen convolution using a 3x3 matrix
 * 
 * @param image
 */
void sharpen(Matrix *mat);

/**
 * Edge detect convolution using a 3x3 matrix
 * 
 * @param image
 */
void edge_detect(Matrix *mat);

/**
 * Gaussian convolution using a 3x3 matrix
 * 
 * @param image
 */
void gauss(Matrix *mat);

/**
 * Gaussian convolution using a 5x5 matrix
 * 
 * @param image
 */
void wide_gauss(Matrix *mat);

#endif // IMAGE_H
