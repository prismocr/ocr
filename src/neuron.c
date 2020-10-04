#include <stdio.h>
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