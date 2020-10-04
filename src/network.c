#include <stdio.h>
#include "network.h"
#include "layer.h"

Network network_new(size_t nb_layers, size_t *sizes) {
    Network network;

    network.nb_layers = nb_layers;
    network.sizes = sizes;
    initialize_layers(&network, nb_layers, sizes);
    network.input_layer = &(network.layers[0]);
    network.output_layer = &(network.layers[nb_layers - 1]);

    return network;
}

void network_free(Network *network) {
    free(network->sizes);
    for (size_t i = 0; i < network->nb_layers; ++i) {
        layer_free(&(network->layers[i]));
    }
    free(network->layers);
}

void initialize_layers(Network *network, size_t nb_layers, size_t *sizes) {
    network->layers = (Layer *) malloc(nb_layers * sizeof(Layer));

    // Initializing Input Layer
    network->layers[0] = layer_new(sizes[0], NULL, &network->layers[1]);

    // Initializing Hidden Layers
    Layer *prev_layer;
    Layer *next_layer;
    for (size_t i = 1; i < nb_layers - 1; ++i) {
        prev_layer = &network->layers[i - 1];
        next_layer = &network->layers[i + 1];
        network->layers[i] = layer_new(sizes[i], prev_layer, next_layer);
    }

    // Initializing Output Layer
    network->layers[nb_layers - 1]
      = layer_new(sizes[nb_layers - 1], &network->layers[nb_layers - 2], NULL);
}

float *network_feed_forward(Network *network, float *input) {
    layer_feed(network->input_layer, input);
    for (size_t i = 1; i < network->nb_layers; i++) {
        layer_front_pop(&(network->layers[i]));
    }
    return network->output_layer->values;
}