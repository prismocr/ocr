#include <stdio.h>
#include <stdlib.h>
#include "string.h"
#include "layer.h"
#include "neuron.h"

Layer layer_new(size_t nb_neurons, Layer *prev_layer, Layer *next_layer) {
    Layer layer;

    layer.prev_layer = prev_layer;
    layer.next_layer = next_layer;

    layer.values = (float *) calloc(nb_neurons, sizeof(float));

    initialize_biases_and_weights(&layer, nb_neurons);

    // Initializing neurons
    layer.nb_neurons = nb_neurons;
    initialize_neurons(&layer);

    return layer;
}

void layer_free(Layer *layer) {
    free(layer->neurons);
    free(layer->biases);
    if (layer->weights.val != NULL)
        matrix_free(&(layer->weights));
    free(layer->values);
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
        matrix_new(nb_neurons, prev_layer->nb_neurons, &(layer->weights));
        matrix_randomize(-1.0f, 1.0f, &(layer->weights));
    }
}

void initialize_neurons(Layer *layer) {
    layer->neurons = (Neuron *) malloc(layer->nb_neurons * sizeof(Neuron));
    for (size_t i = 0; i < layer->nb_neurons; ++i) {
        float *weights_in = get_weights_in(*layer, i);
        layer->neurons[i]
          = neuron_new(&(layer->values[i]), &(layer->biases[i]), weights_in);
    }
}

float *get_weights_in(Layer layer, size_t index) {
    return (layer.prev_layer) ? layer.weights.val[index] : NULL;
}

// float* get_weights_out(Layer layer, size_t index){}

void layer_feed(Layer *layer, float *values) {
    memcpy(layer->values, values, sizeof(float) * layer->nb_neurons);
}

void layer_front_pop(Layer *layer) {
    float new_values[layer->nb_neurons];

    matrix_column_dot(layer->weights, layer->prev_layer->values, new_values);

    for (size_t i = 0; i < layer->nb_neurons; i++) {
        new_values[i] = sigmoid(new_values[i] + layer->biases[i]);
    }

    layer_feed(layer, new_values);
}
