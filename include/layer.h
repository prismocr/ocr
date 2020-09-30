#ifndef LAYER_H
#define LAYER_H

#include <stdlib.h>
#include "neurone.h"

typedef struct Layer Layer;
struct Layer {
    size_t nb_neurones;
    struct Neuron *neurones;
    double *biases;
    // array weights;

    Layer *prev_layer;
    Layer *next_layer;
};

Layer layer_new(size_t nb_neurones, Layer *prev_layer, Layer *next_layer);
void initialize_neurones(Layer *layer);
void layer_free(Layer *layer);

#endif
