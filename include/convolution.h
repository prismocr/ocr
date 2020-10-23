#ifndef CONVOLUTION_H
#define CONVOLUTION_H

#include "matrix.h"

/**
 * Uses a matrix to convolve an image
 * 
 * @param mat matrix, destination
 * @param convo_mat convolution matrix to apply, source
 */
void convolute(Matrix *mat, Matrix *convo_mat);

#endif // CONVOLUTION_H