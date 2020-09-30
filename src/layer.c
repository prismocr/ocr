#include <stdio.h>
#include "layer.h"
#include "neurone.h"

struct Layer new_layer(size_t nb_neurones, struct Layer *prev_layer,
                       struct Layer *next_layer) {
    struct Layer layer;

    layer.prev_layer = prev_layer;
    layer.next_layer = next_layer;

    // Initializing biases
    layer.biases = (double *) malloc(nb_neurones * sizeof(double));

    // Initializing weights
    // layer.weights = brrrrr

    // Initializing neurones
    layer.nb_neurones = nb_neurones;
    initialize_neurones(&layer);

    return layer;
}

void free_layer(struct Layer *layer) {
    printf("free_layer\n");
    free(layer->neurones);
    free(layer->biases);
    // free_array(layer->array);
}

void initialize_neurones(struct Layer *layer) {
    layer->neurones
      = (struct Neurone *) malloc(layer->nb_neurones * sizeof(struct Neurone));
    for (size_t i = 1; i < layer->nb_neurones; ++i) {
        layer->neurones[i] = new_neurone(&(layer->biases[i]), layer, i);
    }
}