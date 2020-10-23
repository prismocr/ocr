#ifndef IMAGE_H
#define IMAGE_H

#include "matrix.h"

// Apply treshold to an image
void image_threshold(float thresh, float maxval, Matrix *image);

// Invert images colors
void image_invert_color(Matrix *image);

// Change contrast level of an image
void image_contrast(Matrix *image, float delta);

// Rotate image by an arbitrary radian angle
void image_rotate(Matrix *image, float angle);

/* BRICE
* Sharpen convolution using a 3x3 matrix
* takes an image pointer
*/
void sharpen(Matrix *mat);

/* BRICE
* Edge detect convolution using a 3x3 matrix
* takes an image pointer
*/
void edge_detect(Matrix *mat);

/* BRICE
* Gaussian convolution using a 3x3 matrix
* takes an image pointer
*/
void gauss(Matrix *mat);

/* BRICE
* Gaussian convolution using a 5x5 matrix
* takes an image pointer
*/
void wide_gauss(Matrix *mat);

#endif // IMAGE_H
