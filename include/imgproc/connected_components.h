#ifndef CONNECTED_COMPONENTS_H
#define CONNECTED_COMPONENTS_H

#include "utils/union_find.h"

typedef struct connected_component ConnectedComponent;

struct connected_component {
    // Bounding box
    size_t x, y, w, h;

    float label;
};

/*
 * Hoshen-Kopelman algorithm that labels connected components.
 */
void cc_labeling_4conn(Matrix *image, UnionFind *u);

void cc_labeling_8conn(Matrix *image, UnionFind *u);

/*
 * Gets image bouding box.
 *
 * @param image
 * @param foreground pixel value.
 * @param x
 * @param y
 * @param w
 * @param h
 */
void cc_bounding_box(Matrix image, float c, size_t *x, size_t *y, size_t *w,
                     size_t *h);

#endif // CONNECTED_COMPONENTS_H
