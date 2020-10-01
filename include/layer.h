#ifndef LAYER_H
#define LAYER_H

#include <stdlib.h>
#include "neurone.h"
#include "matrix.h"

typedef struct Layer Layer;
struct Layer {
    size_t nb_neurones;
    struct Neuron *neurones;
    float *biases;
    Matrix weights;

    Layer *prev_layer;
    Layer *next_layer;
};

Layer layer_new(size_t nb_neurones, Layer *prev_layer, Layer *next_layer);
void layer_free(Layer *layer);

void initialize_biases_and_weights(Layer *layer, size_t nb_neurones);
void initialize_neurones(Layer *layer);

#endif
