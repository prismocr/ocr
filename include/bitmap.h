#ifndef BITMAP_H
#define BITMAP_H

#include "matrix.h"

/*
 * Load an image into a Matrix of grayscale pixels
 */
int load_bmp_image(const char *path, Matrix *image);

/*
 * Save Matrix into bmp image file format.
 */
void save_bmp_image(const char *path, Matrix *image);

#endif // BITMAP_H
