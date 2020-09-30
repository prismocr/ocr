#ifndef NEURONE_H
#define NEURONE_H

#include <stdlib.h>
//#include "array.h"

struct Neurone {
    double *bias;
    double *weights_in;
    double *weights_out;
};

struct Layer;
struct Neurone new_neurone(double *biais, struct Layer *layer, size_t index);

#endif
