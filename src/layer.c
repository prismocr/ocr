#include <stdio.h>
#include "layer.h"
#include "neurone.h"

Layer layer_new(size_t nb_neurones, Layer *prev_layer, Layer *next_layer) {
    Layer layer;

    layer.prev_layer = prev_layer;
    layer.next_layer = next_layer;

    // Initializing biases
    layer.biases = (double *) malloc(nb_neurones * sizeof(double));

    // Initializing weights
    // layer.weights = brrrrr

    // Initializing neurones
    layer.nb_neurones = nb_neurones;
    initialize_neurones(&layer);

    return layer;
}

void layer_free(Layer *layer) {
    free(layer->neurones);
    free(layer->biases);
    // free_array(layer->array);
}

void initialize_neurones(Layer *layer) {
    layer->neurones = (Neuron *) malloc(layer->nb_neurones * sizeof(Neuron));
    for (size_t i = 1; i < layer->nb_neurones; ++i) {
        layer->neurones[i] = neuron_new(&(layer->biases[i]), layer, i);
    }
}