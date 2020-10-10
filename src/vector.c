#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <vector.h>

int vector_new(size_t size, Vector *vector) {
    assert(size > 0);

    vector->size = size;
    vector->values = (float *) calloc(size, sizeof(float));

    return vector->values == NULL;
}

void vector_free(Vector *vector) {
    if (vector->values != NULL)
        free(vector->values);
}

void vector_print(Vector vector) {
    for (size_t i = 0; i < vector.size; i++) {
        printf("%f ", vector.values[i]);
    }
    printf("\n");
}

void vector_printf(const char *format, Vector vector) {
    for (size_t i = 0; i < vector.size; i++) {
        printf(format, vector.values[i]);
    }
    printf("\n");
}

void vector_randomize(Vector *vector, float min, float max) {
    if (min > max) {
        float temp = min;
        min = max;
        max = temp;
    }
    for (size_t i = 0; i < vector->size; i++) {
        vector->values[i] = (float) rand() / RAND_MAX * (max - min) + min;
    }
}

void vector_add(Vector a, Vector b, Vector *res) {
    for (size_t i = 0; i < a.size; i++) {
        res->values[i] = a.values[i] + b.values[i];
    }
}

void vector_sub(Vector a, Vector b, Vector *res) {
    for (size_t i = 0; i < a.size; i++) {
        res->values[i] = a.values[i] - b.values[i];
    }
}

void vector_scale(Vector vector, float scalar, Vector *res) {
    for (size_t i = 0; i < vector.size; i++) {
        res->values[i] = vector.values[i] * scalar;
    }
}

float vector_dot(Vector a, Vector b) {
    float res = 0;
    for (size_t i = 0; i < a.size; i++) {
        res += a.values[i] * b.values[i];
    }
    return res;
}

Vector array_to_vector(float *array, size_t size) {
    Vector vector;
    vector.size = size;
    vector.values = array;
    return vector;
}