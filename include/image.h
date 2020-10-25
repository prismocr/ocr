#ifndef IMAGE_H
#define IMAGE_H

#include "matrix.h"
#define PI 3.14159265359

/**
 * Apply a fixed value binary threshold to every pixel of an image.
 *
 * Formula: (where p(x, y) denotes the pixel at x, y)
 * p(x, y) = | maxval if p(x, y) > thresh
 *           | 0      otherwise
 *
 * @param threshold value.
 * @param maximum value to replace pixel over threshold.
 * @param image to apply threshold on.
 */
void image_threshold(float thresh, float maxval, Matrix *image);

/**
 * Apply a fixed value inverted binary threshold to every pixel of an image.
 *
 * Formula: (where p(x, y) denotes the pixel at x, y)
 * p(x, y) = | 0      if p(x, y) > thresh
 *           | maxval otherwise
 *
 * @param threshold value.
 * @param maximum value to replace pixel below threshold.
 * @param image to apply threshold on.
 */
void image_threshold_inv(float thresh, float maxval, Matrix *image);

/**
 * Invert colors of an image.
 *
 * Formula:
 * p(x, y) = maxval - p(x, y)
 *
 * @param maximum value of the color.
 * @param image to invert color on.
 */
void image_invert_color(float maxval, Matrix *image);

/**
 * Crop a copy of an image.
 *
 * @param x origin of the cropped image.
 * @param y origin of the cropped image.
 * @param width of the cropped image.
 * @param height of the cropped image.
 * @param image to crop.
 */
Matrix image_crop(size_t x, size_t y, size_t w, size_t h, Matrix image);

/**
 *  Adjust image contrast
 *  @param image
 *  @param delta
*/
void image_contrast(Matrix *image, float delta);


/**
 *  Convert an angle in degrees to radians
 *  @param angle
*/
float deg_to_rad(float angle);

/**
 *  Rotate image by an arbitrary radian angle 
 *  @param image
 *  @param angle
*/ 
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
