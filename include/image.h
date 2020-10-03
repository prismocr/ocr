#ifndef IMAGE_H
#define IMAGE_H

#include "matrix.h"

void image_threshold(float thresh, float maxval, Matrix *image);
void image_invert_color(Matrix *image);
void image_contrast(Matrix *image, float delta);
void image_rotate(Matrix *image, float angle);


#endif // IMAGE_H
