#ifndef IMAGE_H
#define IMAGE_H

#include "ndarray.h"

// Load an image in grayscale colorspace into an ndarray
int load_bmp_image(const char *path, ndarray **im);

// read an image and save it
void save_bmp_image(const char *path, ndarray *im);

#endif // IMAGE_H
