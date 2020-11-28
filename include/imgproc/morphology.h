#ifndef MORPHOLOGY_H
#define MORPHOLOGY_H

#include "utils/matrix.h"
#include <stddef.h>

/**
 * Creates a new structuring element of size m x n.
 * It is just a matrix of size m x n filled with 1s.
 *
 * @param number of rows of the structuring element.
 * @param number of columns of the structuring element.
 * @return structuring element of size m x n.
 */
Matrix structuring_element(size_t m, size_t n);

/**
 * Smooth an image by given structuring element.
 *
 * @param image to smooth.
 * @param structuring element used for smoothing.
 */
int smooth(Matrix *image, Matrix kernel);

/**
 * Dilates an image by a given structuring element.
 *
 * param image to dilate.
 * param structuring element used for dilatation.
 */
int dilate(Matrix *image, Matrix kernel);

/**
 * Erodes an image by a given structuring element.
 *
 * param image to erode.
 * param structuring element used for erosion.
 */
int erode(Matrix *image, Matrix kernel);

/**
 * Opens an image by a given structuring element.
 *
 * param image to open.
 * param structuring element used for opening.
 */
void opening(Matrix *image, Matrix kernel);

/**
 * Closes an image by a given structuring element.
 *
 * param image to close.
 * param structuring element used for closing.
 */
void closing(Matrix *image, Matrix kernel);

/**
 * Computes the difference of two images.
 *
 * @param image 1.
 * @param image 2.
 */
void difference(Matrix *image1, Matrix image2);

#endif // MORPHOLOGY_H
