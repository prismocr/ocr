#ifndef VECTOR_H
#define VECTOR_H

typedef struct Vector Vector;
struct Vector {
    size_t size;
    float *val;
};
/**
 * Creates a new vector
 * 
 * @param size of the vector
 * @param pointer of the vector
 * @return state (error)
 */
int vector_new(size_t size, Vector *Vector);
/**
 * Frees a vector
 * 
 * @param vector to free
 */
void vector_free(Vector *vector);

/**
 * Prints a vector
 * @param vector to print
 */
void vector_print(Vector vector);
/**
 * Prints a vector with a special format
 * 
 * @param format to print 
 * @param vector to print
 */
void vector_printf(const char *format, Vector vector);

/**
 * Randomizes values of a vector
 * 
 * @param the vector
 * @param minimum value 
 * @param maximum value
 */
void vector_randomize(Vector *vect, float min, float max);

/**
 * Adds a value to the vector
 * 
 * @param vector for the operation
 * @param a source
 * @param b destination
 */
void vector_add(size_t size, float *a, float *b);
/**
 * Removes a value to the vector
 * 
 * @param vector for the operation
 * @param a source
 * @param b destination
 */
void vector_sub(size_t size, float *a, float *res);
/**
 * Makes a homothety on an array
 * 
 * @param vector for the operation
 * @param scalar scalar
 * @param arr source
 */
void vector_scale(size_t size, float scalar, float *arr);
/**
 * Dot products on arrays
 *
 * @param a source
 * @param b destination
 */
void vector_dot(size_t size, float *a, float *b);

/**
 * Converts arrays to vector
 * 
 * @param array to convert
 * @param size size
 * @return vector
 */
Vector arr2vect(float *array, size_t size);

/**
 * Copies values of a vector to another, changed array is the second
* 
 * @param size size
 * @param a source
 * @param b destination
 */
void vector_copy(size_t size, float *a, float *b);

#endif