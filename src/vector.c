#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include "vector.h"
#include "string.h"

int vector_new(size_t size, Vector *vector) {
    assert(size > 0);

    vector->size = size;
    vector->val = (float *) calloc(size, sizeof(float));

    return vector->val == NULL;
}

void vector_free(Vector *vector) {
    //free(vector->val);
    vector = vector;
}

void vector_print(Vector vector) {
    for (size_t i = 0; i < vector.size; i++) {
        printf("%f ", vector.val[i]);
    }
}

void vector_printf(const char *format, Vector vector) {
    for (size_t i = 0; i < vector.size; i++) {
        printf(format, vector.val[i]);
    }
}

void vector_randomize(Vector *vector, float min, float max) {
    if (min > max) {
        float temp = min;
        min = max;
        max = temp;
    }
    for (size_t i = 0; i < vector->size; i++) {
        vector->val[i] = (float) rand() / RAND_MAX * (max - min) + min;
    }
}

void vector_add(size_t size, float *a, float *b) {
    for (size_t i = 0; i < size; i++) {
        b[i] = b[i] + a[i];
    }
}

void vector_sub(size_t size, float *a, float *b) {
    for (size_t i = 0; i < size; i++) {
        b[i] = a[i] - b[i];
    }
}

void vector_scale(size_t size, float scalar, float *arr) {
    for (size_t i = 0; i < size; i++) {
        arr[i] *= scalar;
    }
}

void vector_dot(size_t size, float *a, float *b) {
    for (size_t i = 0; i < size; i++) {
        b[i] *= a[i];
    }
}

Vector arr2vect(float *array, size_t size) {
    Vector vector;
    vector.size = size;
    vector.val = array;
    return vector;
}

void vector_copy(size_t size, float *a, float *b) {
    memcpy(b, a, sizeof(float) * size);
}