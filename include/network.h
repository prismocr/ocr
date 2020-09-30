#ifndef NETWORK_H
#define NETWORK_H

#include <stdlib.h>

typedef struct Layer Layer;

typedef struct Network Network;
struct Network {
    size_t nb_layers; // Number of layers within the network
    size_t *sizes;    // List containing length of each layer
    Layer *layers;
};

Network network_new(size_t nb_layers, size_t *sizes);
void network_free(Network *network);

#endif
