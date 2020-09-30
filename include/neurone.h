#ifndef NEURONE_H
#define NEURONE_H

#include <stdlib.h>

typedef struct Neuron Neuron;
struct Neuron {
    double *bias;
    double **weights_in;
    double **weights_out;
};

typedef struct Layer Layer;
Neuron neuron_new(double *biais, Layer *layer, size_t index);

#endif
