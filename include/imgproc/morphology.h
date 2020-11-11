#ifndef MORPHOLOGY_H
#define MORPHOLOGY_H

#include "utils/matrix.h"
#include <stddef.h>

Matrix structuring_element(size_t m, size_t n);

void smooth(Matrix *image, Matrix kernel);
void dilate(Matrix *image, Matrix kernel);
void erode(Matrix *image, Matrix kernel);
void opening(Matrix *image, Matrix kernel);
void closing(Matrix *image, Matrix kernel);
void difference(Matrix *image1, Matrix image2);

#endif // MORPHOLOGY_H
