#ifndef NEURONE_H
#define NEURONE_H

#include <stdlib.h>

typedef struct Neuron Neuron;
struct Neuron {
    float *value;
    float *bias;
    float *weights_in;
};

typedef struct Layer Layer;
Neuron neuron_new(float *value, float *bias, float *weights_in);

float relu(float x);
float sigmoid(float x);
float sigmoid_prime(float z);

#endif
