#ifndef IMAGE_H
#define IMAGE_H

#include "matrix.h"

/**
 *  Apply theshold on an image.
 *  @param thresh
 *  @param maxval
 *  @param image 
 * 
*/
void image_threshold(float thresh, float maxval, Matrix *image);

/**
 *  Invert colors of an image 
 *  @param image
*/
void image_invert_color(Matrix *image);

/**
 *  Adjust image contrast
 *  @param image
 *  @param delta
*/
void image_contrast(Matrix *image, float delta);

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
