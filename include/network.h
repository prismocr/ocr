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

/**
 * Creates a new network
 * 
 * @param number of layers of the network
 * @param size of each layer
 * @result network created
 */
Network network_new(size_t nb_layers, size_t *sizes);
/**
 * Frees a network
 * 
 * @param network to free
 */
void network_free(Network *network);

/**
 * Initializes layers of a network
 * 
 * @param network containing the layers
 * @param number of layers of the network
 * @param size of each layer
 */
void initialize_layers(Network *network, size_t nb_layers, size_t *sizes);

/**
 * Applies feedforward on a network with a given input
 * 
 * @param the network
 * @param the inputs
 * @result the outputs of the network
 */
float *network_feed_forward(Network *network, float *input);
/**
 * Trains a network with stochastic gradient descent
 * 
 * @param the network
 * @param dataset to train on
 * @param number of epochs for training
 * @param size of the batches that will be extracted from dataset
 * @param the learning rate
 */
void network_sgd(Network *network, Dataset *dataset, size_t epochs,
                 size_t batch_size, float learning_rate);
/**
 * Applies gradient descent on a network 
 * 
 * @param the network
 * @param the inputs
 * @param the learning rate
 */
void batch_gd(Network *network, Dataset *batch, float learning_rate);
/**
 * Applies backpropagation on a network
 * 
 * @param the network
 * @param the data used as input
 */
void network_backpropagation(Network *network, Data data);
/**
 * Initializes cost of the output layer for backpropagation
 * 
 * @param the data used as input
 * @param expected result of the network
 */
void init_cost(Layer *out_layer, float *target);
/**
 * Applies changes to weights and biases with values of deltas
 * Resets the deltas
 * 
 * @param the network
 * @param size of the batch the network has been trained on
 * @param the learning rate
 */
void apply_grad(Network *network, size_t size_batch, float learning_rate);

/**
 * Prints all datas of a network
 * 
 * @param network to print
 */
void network_print(Network network);

/**
 * Prints only weights and biases
 * 
 * @param network to print
 */
void network_print_clean(Network network);
/**
 * Prints the results of a network with a given dataset
 * 
 * @param network to print
 */
void network_print_results(Network network, Dataset dataset);

/**
 * Saves a network in a file
 * 
 * @param path of the file
 * @param network to save
 */
void network_save(const char *path, Network network);
/**
 * Loads a network which is in a file
 * 
 * @param path of the file
 * @param network
 * @return state (error)
 */
int network_load(const char *path, Network *out);

#endif
