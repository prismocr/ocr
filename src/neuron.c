#include <stdio.h>
#include <math.h>
#include "neuron.h"
#include "layer.h"

typedef struct Neuron Neuron;
Neuron neuron_new(float *value, float *bias, float *weights_in) {
    Neuron neuron;

    neuron.value = value;
    neuron.bias = bias;
    neuron.weights_in = weights_in;

    return neuron;
}

float relu(float x) {
    return (x < 0) ? 0 : x;
}

float sigmoid(float x) {
    return 1.f / (1 + expf(-x));
}