#include "neurone.h"
#include "layer.h"
#include <stdio.h>

struct Neurone new_neurone(double *biaise, struct Layer *layer, size_t index) {
    struct Neurone neurone;

    neurone.bias = biaise;
    // link weights_in
    // link weights_out

    return neurone;
}