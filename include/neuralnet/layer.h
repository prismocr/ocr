#ifndef LAYER_H
#define LAYER_H

#include <stdlib.h>
#include "neuralnet/neuron.h"
#include "utils/matrix.h"

typedef struct Layer Layer;
typedef void (*l2v)(Layer *z);
typedef float (*f2f)(float z);
struct Layer {
    size_t nb_neurons;
    struct Neuron *neurons;
    float *values;
    float *biases;
    Matrix weights;

    float *z;

    float *deltas;
    float *d_biases;
    Matrix d_weights;

    l2v layer_act_func;
    f2f act_func_prime;
    Layer *prev_layer;
    Layer *next_layer;
};

/**
 * Creates a random new layer
 *
 * @param number of neurons within the layer
 * @param previous layer for linking
 * @param next layer for linking
 * @param activation function
 * @param derivative of activation function
 * @return layer created
 */
Layer layer_new(size_t nb_neurons, Layer *prev_layer, Layer *next_layer,
                int act_func);
/**
 * Frees a layer
 *
 * @param layer to free
 */
void layer_free(Layer *layer);

/**
 * Initializes biases and weights of a layer to random values within -1 and 1
 *
 * @param layer to initialize
 * @param number of neurons of the layer
 */
void initialize_biases_and_weights(Layer *layer);
/**
 * Initializes deltas of layer to 0, used for stochastic gradient descent
 *
 * @param layer to initialize
 */
void initialize_deltas(Layer *layer);
/**
 * Initializes deltas of layer to 0, used for stochastic gradient descent
 *
 * @param layer to initialize
 */
void initialize_neurons(Layer *layer);

/**
 * Returns weights coming into a neuron
 *
 * @param layer holding the neuron
 * @param index of the neuron within the layer
 * @return array of weights
 */
float *get_weights_in(Layer layer, size_t index);

void layer_sigmoid(Layer *layer);

void layer_relu(Layer *layer);

void layer_softmax(Layer *layer);
/**
 * Feeds a layer with activation values
 *
 * @param layer to feed
 * @param values to feed
 */
void layer_feed(Layer *layer, float *values);
/**
 * Executes a step of the feed forward
 *
 * @param  layer to front pop
 */
void layer_front_pop(Layer *layer);
/**
 * Executes backpropagation on a layer updating its deltas
 *
 * @param layer to apply backpropagation on
 */
void layer_backpropagation(Layer *layer);
#endif
