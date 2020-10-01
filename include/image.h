#ifndef IMAGE_H
#define IMAGE_H

#include "matrix.h"

// Load an image in grayscale colorspace into an ndarray
int load_bmp_image(const char *path, Matrix *image);

// read an image and save it
void save_bmp_image(const char *path, Matrix *image);

#endif // IMAGE_H

