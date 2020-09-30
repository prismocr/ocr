#include <stdio.h>
#include "network.h"
#include "layer.h"

struct Network new_network(size_t nb_layers, size_t *sizes) {
    // instantiating network struct
    struct Network network;

    // filling simple properties : number of layers, number of neurons per
    // layer, and the layer array itself
    network.nb_layers = nb_layers;
    network.sizes = sizes;
    network.layers = (struct Layer *) malloc(nb_layers * sizeof(struct Layer));

    // Initializing layers array
    // Input Layer
    network.layers[0] = new_layer(sizes[0], NULL, &network.layers[1]);
    // Hidden Layers
    for (size_t i = 1; i < nb_layers - 1; ++i) {
        network.layers[i]
          = new_layer(sizes[i], &network.layers[i - 1], &network.layers[i + 1]);
    }
    // Output Layer
    network.layers[nb_layers - 1]
      = new_layer(sizes[nb_layers - 1], &network.layers[nb_layers - 2], NULL);

    // we gud to go
    return network;
}

void free_network(struct Network *network) {
    free(network->sizes);
    for (size_t i = 0; i < network->nb_layers; ++i) {
        free_layer(&(network->layers[i]));
    }
    free(network->layers);
}