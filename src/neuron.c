#include <stdio.h>
#include "neuron.h"
#include "layer.h"

typedef struct Neuron Neuron;
Neuron neuron_new(float *bias, float *weights_in) {
    Neuron neuron;

    neuron.value = 0.0f;
    neuron.bias = bias;
    neuron.weights_in = weights_in;

    return neuron;
}