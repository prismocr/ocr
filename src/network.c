#include <stdio.h>
#include "network.h"
#include "layer.h"

Network network_new(size_t nb_layers, size_t *sizes) {
    // instantiating network struct
    Network network;

    // filling simple properties : number of layers, nbneurons/layer,
    // and the layer array itself
    network.nb_layers = nb_layers;
    network.sizes = sizes;
    network.layers = (Layer *) malloc(nb_layers * sizeof(Layer));

    // Initializing layers array
    // Input Layer
    network.layers[0] = layer_new(sizes[0], NULL, &network.layers[1]);
    // Hidden Layers
    Layer *prev_layer;
    Layer *next_layer;
    for (size_t i = 1; i < nb_layers - 1; ++i) {
        prev_layer = &network.layers[i - 1];
        next_layer = &network.layers[i + 1];
        network.layers[i] = layer_new(sizes[i], prev_layer, next_layer);
    }
    // Output Layer
    network.layers[nb_layers - 1]
      = layer_new(sizes[nb_layers - 1], &network.layers[nb_layers - 2], NULL);

    // we gud to go
    return network;
}

void network_free(Network *network) {
    free(network->sizes);
    for (size_t i = 0; i < network->nb_layers; ++i) {
        layer_free(&(network->layers[i]));
    }
    free(network->layers);
}