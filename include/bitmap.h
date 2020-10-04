#ifndef BITMAP_H
#define BITMAP_H

#include "matrix.h"

/*
 * Load an image into a Matrix of grayscale pixels
 */
int bitmap_load(const char *path, Matrix *image);

/*
 * Save Matrix into bmp image file format.
 */
int bitmap_save(const char *path, Matrix *image);

#endif // BITMAP_H
