#ifndef NEURONE_H
#define NEURONE_H

#include <stdlib.h>

typedef struct Neuron Neuron;
struct Neuron {
    float *value;
    float *bias;
    float *weights_in;
};

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
 * @param x
 * @result relu(x)
 */
float relu(float x);
/**
 * Sigmoid activation function
 * 
 * @param x
 * @result sigmoid(x)
 */
float sigmoid(float x);
/**
 * Derivative of sigmoid
 * 
 * @param z
 * @result sigmoid'(z)
 */
float sigmoid_prime(float z);

#endif
