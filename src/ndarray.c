#include <stdio.h>
#include "ndarray.h"

void ndarray_print(const ndarray *a) {
    
}

size_t ndarray_size(const ndarray *a) {
    size_t size = 0;

    for (int i = 0; i < a->ndim; i++) {
        size += a->dim[i];
    }

    return size;
}

