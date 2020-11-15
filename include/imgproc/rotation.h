#ifndef ROTATION_H
#define ROTATION_H

#include "utils/matrix.h"

#define PI 3.14159265359

/**
 *  Convert an angle in degrees to radians
 *  @param angle
 */
float deg_to_rad(float angle);

/**
 *  Convert an angle in radians to degrees
 *  @param angle
 */
float rad_to_deg(float angle);
/**
 *  Rotate image by an arbitrary radian angle
 *  @param image
 *  @param angle
 */
void image_rotate(Matrix *image, float angle);

/**
 *  Detect image rotation angle with given precision
 *  This algorithm uses Hough Transformations.
 *  https://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.741.4980&rep=rep1&type=pdf
 *  @param image
 *  @param precision
 */
float image_detect_skew(Matrix *image, float precision);

/**
 *  Automatically rotate an image.
 *  @param image
 *  @param precision
 */
void image_auto_rotate(Matrix *image, float precision);

#endif // ROTATION_H
