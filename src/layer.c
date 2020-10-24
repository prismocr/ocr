#include <stdio.h>
#include <stdlib.h>
#include "string.h"
#include "layer.h"
#include "neuron.h"
#include "vector.h"

Layer layer_new(size_t nb_neurons, Layer *prev_layer, Layer *next_layer) {
    Layer layer;

    layer.nb_neurons = nb_neurons;
    layer.prev_layer = prev_layer;
    layer.next_layer = next_layer;

    layer.values = (float *) calloc(nb_neurons, sizeof(float));
    initialize_biases_and_weights(&layer);
    initialize_deltas(&layer);

    initialize_neurons(&layer);

    return layer;
}

void layer_free(Layer *layer) {
    if (layer->prev_layer != NULL) {
        free(layer->biases);
        free(layer->d_biases);

        matrix_free(&(layer->weights));
        matrix_free(&(layer->d_weights));

        free(layer->deltas);
        free(layer->z);
    }
    free(layer->neurons);
    free(layer->values);
}

void initialize_biases_and_weights(Layer *layer) {
    size_t nb_neurons = layer->nb_neurons;
    if (layer->prev_layer == NULL) {
        layer->biases = NULL;
        layer->weights.val = NULL;
    } else {
        // Initializing biases
        layer->biases = (float *) malloc(nb_neurons * sizeof(float));
        for (size_t i = 0; i < nb_neurons; i++) {
            layer->biases[i] = 2 * ((float) rand() / RAND_MAX) - 1;
        }

        // Initializing weights
        matrix_new(nb_neurons, layer->prev_layer->nb_neurons,
                   &(layer->weights));
        matrix_randomize(-1.0f, 1.0f, &(layer->weights));
    }
}

void initialize_deltas(Layer *layer) {
    size_t nb_neurons = layer->nb_neurons;
    if (layer->prev_layer == NULL) {
        layer->deltas = NULL;
        layer->d_weights.val = NULL;
        layer->d_biases = NULL;
        layer->z = NULL;
    } else {
        layer->deltas = (float *) calloc(nb_neurons, sizeof(float));
        matrix_new(nb_neurons, layer->prev_layer->nb_neurons,
                   &(layer->d_weights));
        layer->d_biases = (float *) malloc(nb_neurons * sizeof(float));
        layer->z = (float *) calloc(nb_neurons, sizeof(float));
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

void layer_feed(Layer *layer, float *values) {
    memcpy(layer->values, values, sizeof(float) * layer->nb_neurons);
}

void layer_front_pop(Layer *layer) {
    // weighted sum
    matrix_column_dot(layer->weights, layer->prev_layer->values, layer->z);
    vector_add(layer->nb_neurons, layer->biases, layer->z);

    // activation function
    for (size_t i = 0; i < layer->nb_neurons; i++) {
        layer->values[i] = sigmoid(layer->z[i]);
    }
}

void layer_backpropagation(Layer *layer) {
    size_t nb_neurons = layer->nb_neurons;
    Layer *next_layer = layer->next_layer;

    // Compute new deltas from previous ones
    matrix_cdt(next_layer->weights, next_layer->deltas, layer->deltas);
    for (size_t i = 0; i < nb_neurons; i++) {
        layer->deltas[i] *= sigmoid_prime(layer->z[i]);
    }

    // Apply cost to biases
    vector_add(nb_neurons, layer->deltas, layer->d_biases);
    // Compute differential for weights
    matrix_dcl(nb_neurons, layer->deltas, layer->prev_layer->nb_neurons,
               layer->prev_layer->values, &layer->d_weights);
}