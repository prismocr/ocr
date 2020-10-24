#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include "vector.h"
#include "string.h"

/**
 * Creates a new vector
 * 
 * @param size of the vector
 * @param pointer of the vector
 * @return state (error)
 */
int vector_new(size_t size, Vector *vector) {
    assert(size > 0);

    vector->size = size;
    vector->val = (float *) calloc(size, sizeof(float));

    return vector->val == NULL;
}

/**
 * Frees a vector
 * 
 * @param vector to free
 */
void vector_free(Vector *vector) {
    if(vector->val){
        free(vector->val);
        vector->val=NULL;
    }
}

/**
 * Prints a vector
 * @param vector to print
 */
void vector_print(Vector vector) {
    for (size_t i = 0; i < vector.size; i++) {
        printf("%f ", vector.val[i]);
    }
}

/**
 * Prints a vector with a special format
 * 
 * @param format to print 
 * @param vector to print
 */
void vector_printf(const char *format, Vector vector) {
    for (size_t i = 0; i < vector.size; i++) {
        printf(format, vector.val[i]);
    }
}

/**
 * Randomizes values of a vector
 * 
 * @param the vector
 * @param minimum value 
 * @param maximum value
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
 * 
 * @param vector for the operation
 * @param a source
 * @param b destination
 */
void vector_add(size_t size, float *a, float *b) {
    for (size_t i = 0; i < size; i++) {
        b[i] = b[i] + a[i];
    }
}

/**
 * Removes a value to the vector
 * 
 * @param vector for the operation
 * @param a source
 * @param b destination
 */
void vector_sub(size_t size, float *a, float *b) {
    for (size_t i = 0; i < size; i++) {
        b[i] = a[i] - b[i];
    }
}

/**
 * Makes a homothety on an array
 * 
 * @param vector for the operation
 * @param scalar scalar
 * @param arr source
 */
void vector_scale(size_t size, float scalar, float *arr) {
    for (size_t i = 0; i < size; i++) {
        arr[i] *= scalar;
    }
}

/**
 * Dot products on arrays
 *
 * @param a source
 * @param b destination
 */
void vector_dot(size_t size, float *a, float *b) {
    for (size_t i = 0; i < size; i++) {
        b[i] *= a[i];
    }
}

/**
 * Converts arrays to vector
 * 
 * @param array to convert
 * @param size size
 * @return vector
 */
Vector arr2vect(float *array, size_t size) {
    Vector vector;
    vector.size = size;
    vector.val = array;
    return vector;
}

/**
 * Copies values of a vector to another, changed array is the second
* 
 * @param size size
 * @param a source
 * @param b destination
 */
void vector_copy(size_t size, float *a, float *b) {
    memcpy(b, a, sizeof(float) * size);
}