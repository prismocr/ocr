#include <stdio.h>
#include <stdlib.h>
#include "layer.h"
#include "neurone.h"

Layer layer_new(size_t nb_neurones, Layer *prev_layer, Layer *next_layer) {
    Layer layer;

    layer.prev_layer = prev_layer;
    layer.next_layer = next_layer;

    initialize_biases_and_weights(&layer, nb_neurones);

    // Initializing neurones
    layer.nb_neurones = nb_neurones;
    initialize_neurones(&layer);

    return layer;
}

void layer_free(Layer *layer) {
    free(layer->neurones);
    free(layer->biases);
    matrix_free(&(layer->weights));
}

void initialize_biases_and_weights(Layer *layer, size_t nb_neurones) {
    // Initializing biases
    layer->biases = (float *) malloc(nb_neurones * sizeof(float));
    for (size_t i = 0; i < nb_neurones; i++) {
        layer->biases[i] = 2 * ((float) rand() / RAND_MAX) - 1;
    }

    // Initializing weights
    layer->weights.val = NULL;
    Layer *prev_layer = layer->prev_layer;
    if (prev_layer != NULL) {
        layer->weights = matrix_new(nb_neurones, prev_layer->nb_neurones);
        matrix_randomize(&(layer->weights));
    }
    // do matrix stuff (I need to push matrix before making this part go brrr)
}

void initialize_neurones(Layer *layer) {
    layer->neurones = (Neuron *) malloc(layer->nb_neurones * sizeof(Neuron));
    for (size_t i = 1; i < layer->nb_neurones; ++i) {
        layer->neurones[i] = neuron_new(&(layer->biases[i]), layer, i);
    }
}