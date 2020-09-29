#ifndef NDARRAY_H
#define NDARRAY_H

#include <stddef.h>

typedef struct ndarray ndarray;

struct ndarray {
    size_t ndim;
    size_t *dim;
    float *val;
};

void ndarray_print(const ndarray *a);

#endif // NDARRAY_H

