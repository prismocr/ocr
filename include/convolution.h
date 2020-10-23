#ifndef CONVOLUTION_H
#define CONVOLUTION_H

#include "matrix.h"

/* BRICE
* Uses a matrix to convolve an image
* takes a matrix pointer (image) and a convolution matrix (pointer)
*/
void convolute(Matrix *mat, Matrix *convo_mat);

#endif // CONVOLUTION_H