#ifndef CONNECTED_COMPONENTS_H
#define CONNECTED_COMPONENTS_H

/* Two pass 4-direction connected components labeling
 * Pixel labels are val - 255.f
 */
void connected_components_labeling(Matrix *image, float *label);

#endif // CONNECTED_COMPONENTS_H
