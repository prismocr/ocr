#include <stdio.h>
#include <stdlib.h>
#include "layer.h"
#include "neuron.h"

Layer layer_new(size_t nb_neurons, Layer *prev_layer, Layer *next_layer) {
    Layer layer;

    layer.prev_layer = prev_layer;
    layer.next_layer = next_layer;

    initialize_biases_and_weights(&layer, nb_neurons);

    // Initializing neurons
    layer.nb_neurons = nb_neurons;
    initialize_neurons(&layer);

    return layer;
}

void layer_free(Layer *layer) {
    free(layer->neurons);
    free(layer->biases);
    matrix_free(&(layer->weights));
}

void initialize_biases_and_weights(Layer *layer, size_t nb_neurons) {
    // Initializing biases
    layer->biases = (float *) malloc(nb_neurons * sizeof(float));
    for (size_t i = 0; i < nb_neurons; i++) {
        layer->biases[i] = 2 * ((float) rand() / RAND_MAX) - 1;
    }

    // Initializing weights
    layer->weights.val = NULL;
    Layer *prev_layer = layer->prev_layer;
    if (prev_layer != NULL) {
        layer->weights = matrix_new(nb_neurons, prev_layer->nb_neurons);
        matrix_randomize(&(layer->weights));
    }
}

void initialize_neurons(Layer *layer) {
    layer->neurons = (Neuron *) malloc(layer->nb_neurons * sizeof(Neuron));
    for (size_t i = 0; i < layer->nb_neurons; ++i) {
        float *weights_in = get_weights_in(*layer, i);
        layer->neurons[i] = neuron_new(&(layer->biases[i]), weights_in);
    }
}

float *get_weights_in(Layer layer, size_t index) {
    return (layer.prev_layer) ? layer.weights.val[index] : NULL;
}

// float* get_weights_out(Layer layer, size_t index){}
