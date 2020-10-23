#ifndef VECTOR_H
#define VECTOR_H

typedef struct Vector Vector;
struct Vector {
    size_t size;
    float *val;
};
/**
 * Creates a new vector
 * takes the size of the vector, a pointer to the vector, return state (succeed)
 */
int vector_new(size_t size, Vector *Vector);
/**
 * Frees a vector
 * takes the vector to free
 */
void vector_free(Vector *vector);

/**
 * Prints a vector
 * takes the vector to print
 */
void vector_print(Vector vector);
/**
 * Prints a vector with a special format
 * takes the format to print and the vector to print
 */
void vector_printf(const char *format, Vector vector);

/**
 * Randomizes values of a vector
 * takes the vector, a minimum value and a maximum value
 */
void vector_randomize(Vector *vect, float min, float max);

/**
 * Adds a value to the vector
 * takes the vector for the operation and two floats
 */
void vector_add(size_t size, float *a, float *b);
/**
 * Removes a value to the vector
 * takes the vector for the operation and two floats
 */
void vector_sub(size_t size, float *a, float *res);
/**
 * Makes a homothety on an array
 * takes the size of the vector, a scalar and an array
 */
void vector_scale(size_t size, float scalar, float *arr);
/**
 * Dots products on arrays
 * takes the size of the arrays and two arrays
 */
void vector_dot(size_t size, float *a, float *b);

/**
 * Converts arrays to vector
 * takes an array and the size of the vector, return a vector
 */
Vector arr2vect(float *array, size_t size);

/**
 * Copies values of a vector to another
 * takes the size of the vector, and two vector
 */
void vector_copy(size_t size, float *a, float *b);

#endif