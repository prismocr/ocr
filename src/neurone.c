#include <stdio.h>
#include "neurone.h"
#include "layer.h"

typedef struct Neuron Neuron;
Neuron neuron_new(double *bias, Layer *layer, size_t index) {
    Neuron neurone;

    neurone.bias = bias;
    // link weights_in
    // link weights_out

    return neurone;
}