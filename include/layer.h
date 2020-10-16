#ifndef LAYER_H
#define LAYER_H

#include <stdlib.h>
#include "neuron.h"
#include "matrix.h"

typedef struct Layer Layer;
struct Layer {
    size_t nb_neurons;
    struct Neuron *neurons;
    float *values;
    float *biases;
    Matrix weights;

    float *z;

    float *deltas;
    float *d_biases;
    Matrix d_weights;

    Layer *prev_layer;
    Layer *next_layer;
};

Layer layer_new(size_t nb_neurons, Layer *prev_layer, Layer *next_layer);
void layer_free(Layer *layer);

void initialize_biases_and_weights(Layer *layer, size_t nb_neurons);
void initialize_deltas(Layer *layer, size_t nb_neurons);
void initialize_neurons(Layer *layer);

float *get_weights_in(Layer layer, size_t index);

void layer_feed(Layer *layer, float *values);
void layer_front_pop(Layer *layer);
void layer_backpropagation(Layer *layer);
#endif
