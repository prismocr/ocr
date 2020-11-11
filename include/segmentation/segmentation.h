#ifndef SEGMENTATION_H
#define SEGMENTATION_H

#include "utils/matrix.h"

void segment_rlsa(Matrix image);

void segment_morph_hist(Matrix image);
void feature_extract_morph_based(Matrix *image);

void segment_water_flow(Matrix image);
void morphological_preproc(Matrix *image);

#endif // SEGMENTATION_H
