#ifndef NEURONE_H
#define NEURONE_H

#include <stdlib.h>

typedef struct Neuron Neuron;
struct Neuron {
    float *value;
    float *bias;
    float *weights_in;
};

enum { SIGMOID, RELU };
/**
 * Creates a new neuron
 *
 * @param activation values
 * @param biases
 * @param weights coming in the the neuron
 * @result the neuron created
 */
Neuron neuron_new(float *value, float *bias, float *weights_in);

/**
 * Relu activation function
 *
 * @param z
 * @result relu(z)
 */
float relu(float z);
/**
 * Derivative of relu
 *
 * @param z
 * @result relu'(z)
 */
float relu_prime(float z);
/**
 * Sigmoid activation function
 *
 * @param z
 * @result sigmoid(z)
 */
float sigmoid(float z);
/**
 * Derivative of sigmoid
 *
 * @param z
 * @result sigmoid'(z)
 */
float sigmoid_prime(float z);

#endif
