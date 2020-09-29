#ifndef NEURONE_H
#define NEURONE_H

//#include "array.h"
#include <stdlib.h>

struct Neurone {
    double *bias;
    double *weights_in;
    double *weights_out;
};

struct Layer;
struct Neurone new_neurone(double *biais, struct Layer *layer, size_t index);

#endif
