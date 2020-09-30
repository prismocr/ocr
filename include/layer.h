#ifndef LAYER_H
#define LAYER_H

#include <stdlib.h>
#include "neurone.h"

struct Layer {
    size_t nb_neurones;
    struct Neurone *neurones;
    double *biases;
    // array weights;

    struct Layer *prev_layer;
    struct Layer *next_layer;
};

struct Layer new_layer(size_t nb_neurones, struct Layer *prev_layer,
                       struct Layer *next_layer);
void initialize_neurones(struct Layer *layer);
void free_layer(struct Layer *layer);

#endif
