#ifndef NEURONE_H
#define NEURONE_H

#include <stdlib.h>

typedef struct Neuron Neuron;
struct Neuron {
    float *bias;
    float **weights_in;
    float **weights_out;
};

typedef struct Layer Layer;
Neuron neuron_new(float *biais, Layer *layer, size_t index);

#endif
