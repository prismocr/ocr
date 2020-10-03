#ifndef CONVOLUTION_H
#define CONVOLUTION_H

#include "matrix.h"

// Convolution a matrix
void convolute(Matrix *mat, Matrix *convo_mat);

// Sharpen a matrix
void sharpen(Matrix *mat);
void edge_detect(Matrix *mat);
void gauss(Matrix *mat);
void wide_gauss(Matrix *mat);

#endif // CONVOLUTION_H