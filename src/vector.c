#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include "vector.h"
#include "string.h"

/**
 * Creates a new vector
 * takes the size of the vector, a pointer to the vector, return state (succeed)
 */
int vector_new(size_t size, Vector *vector) {
    assert(size > 0);

    vector->size = size;
    vector->val = (float *) calloc(size, sizeof(float));

    return vector->val == NULL;
}

/**
 * Frees a vector
 * takes the vector to free
 */
void vector_free(Vector *vector) {
    if(vector->val){
        free(vector->val);
        vector->val=NULL;
    }
}

/**
 * Prints a vector
 * takes the vector to print
 */
void vector_print(Vector vector) {
    for (size_t i = 0; i < vector.size; i++) {
        printf("%f ", vector.val[i]);
    }
}

/**
 * Prints a vector with a special format
 * takes the format to print and the vector to print
 */
void vector_printf(const char *format, Vector vector) {
    for (size_t i = 0; i < vector.size; i++) {
        printf(format, vector.val[i]);
    }
}

/**
 * Randomizes values of a vector
 * takes the vector, a minimum value and a maximum value
 */
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

/**
 * Adds a value to the vector
 * takes the vector for the operation and two floats
 */
void vector_add(size_t size, float *a, float *b) {
    for (size_t i = 0; i < size; i++) {
        b[i] = b[i] + a[i];
    }
}

/**
 * Removes a value to the vector
 * takes the vector for the operation and two floats
 */
void vector_sub(size_t size, float *a, float *b) {
    for (size_t i = 0; i < size; i++) {
        b[i] = a[i] - b[i];
    }
}

/**
 * Makes a homothety on an array
 * takes the size of the vector, a scalar and an array
 */
void vector_scale(size_t size, float scalar, float *arr) {
    for (size_t i = 0; i < size; i++) {
        arr[i] *= scalar;
    }
}

/**
 * Dot products on arrays
 * takes the size of the arrays and two arrays
 */
void vector_dot(size_t size, float *a, float *b) {
    for (size_t i = 0; i < size; i++) {
        b[i] *= a[i];
    }
}

/**
 * Converts arrays to vector
 * takes an array and the size of the vector, return a vector
 */
Vector arr2vect(float *array, size_t size) {
    Vector vector;
    vector.size = size;
    vector.val = array;
    return vector;
}

/**
 * Copies values of a vector to another, changed array is the second
 * takes the size of the vector, and two vector
 */
void vector_copy(size_t size, float *a, float *b) {
    memcpy(b, a, sizeof(float) * size);
}