#ifndef BITMAP_H
#define BITMAP_H

#include "matrix.h"

/**
 * Loads an image into a Matrix of grayscale pixels
 * 
 * @param path to load the file
 * @param image to load
 * @return state (error)
 */
int bitmap_load(const char *path, Matrix *image);

/**
 * Save Matrix into bmp image file format.
 * 
 * @param path to save the file
 * @param image to save
 * @return state (error)
 */
int bitmap_save(const char *path, Matrix *image);

#endif // BITMAP_H
