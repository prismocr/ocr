#ifndef NETWORK_H
#define NETWORK_H

#include <stdlib.h>
#include "layer.h"
#include "data.h"

typedef struct Network Network;
struct Network {
    size_t nb_layers; // Number of layers within the network
    Layer *layers;    // Dynamic array of layers
    Layer *input_layer;
    Layer *output_layer;
};

Network network_new(size_t nb_layers, size_t *sizes);
void network_free(Network *network);

void initialize_layers(Network *network, size_t nb_layers, size_t *sizes);

float *network_feed_forward(Network *network, float *input);
void network_sgd(Network *network, Dataset *dataset, size_t epochs,
                 size_t batch_size, float learning_rate);
void batch_gd(Network *network, Dataset *batch);
void network_backpropagation(Network *network, Data data);
void init_cost(Layer *out_layer, float *output, float *target);
void apply_grad(Network *network, size_t size_batch, float learning_rate);

void network_print(Network network);
void network_print_clean(Network network);
void network_print_results(Network network, Dataset dataset);

void network_save(const char *path, Network network);
int network_load(const char *path, Network *out);

#endif
