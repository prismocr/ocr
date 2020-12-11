#ifndef CONNECTED_COMPONENTS_H
#define CONNECTED_COMPONENTS_H

#include "utils/union_find.h"

/*
 * Hoshen-Kopelman algorithm that labels connected components.
 */
void cc_labeling(Matrix *image, UnionFind *u);

#endif // CONNECTED_COMPONENTS_H
