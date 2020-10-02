#ifndef IMAGE_H
#define IMAGE_H

#include "matrix.h"

void image_threshold(float thresh, float maxval, Matrix *image);
void image_invert_color(Matrix *image);

#endif // IMAGE_H

