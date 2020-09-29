#ifndef NETWORK_H
#define NETWORK_H

#include <stdlib.h>

struct Layer;
//#include "layer.h"

struct Network {
    size_t nb_layers; // Number of layers within the network
    size_t *sizes;    // List containing length of each layer
    struct Layer *layers;
};

struct Network new_network(size_t nb_layers, size_t *sizes);
void free_network(struct Network *network);

#endif
