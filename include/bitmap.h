#ifndef BITMAP_H
#define BITMAP_H

#include "matrix.h"

/**
 * Loads an image into a Matrix of grayscale pixels
 * takes a path of the file, a Matrix pointer, return int (succeed)
 */
int bitmap_load(const char *path, Matrix *image);

/**
 * Saves Matrix into bmp image file format.
 * takes the path to save, an image, return int (succeed)
 */
int bitmap_save(const char *path, Matrix *image);

#endif // BITMAP_H
