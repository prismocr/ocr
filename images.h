#include <stdio.h>

#ifndef IMAGES_H_
#define IMAGES_H_

struct image {
    size_t ndim;
    size_t dimensions[2];
    double *values;
};

struct image load_image_grey(char path[]); // load an image and return the struct of it
void save_image(char path[], char path_grey[], struct image im); // read an image and save it

#endif // IMAGES_H_