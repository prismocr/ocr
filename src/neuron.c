#include <stdio.h>
#include <math.h>
#include "neuron.h"
#include "layer.h"

Neuron neuron_new(float *value, float *bias, float *weights_in) {
    Neuron neuron;

    neuron.value = value;
    neuron.bias = bias;
    neuron.weights_in = weights_in;

    return neuron;
}

float relu(float z) {
    return (z < 0) ? 0 : z;
}

float relu_prime(float z){
	return (z > 0) ? 1 : 0;
}

float sigmoid(float z) {
    return 1.f / (1 + expf(-z));
}

float sigmoid_prime(float z) {
    return sigmoid(z) * (1 - sigmoid(z));
}