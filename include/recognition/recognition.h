#ifndef RECOGNITION_H
#define RECOGNITION_H

#include "neuralnet/network.h"
#include "segmentation/segmentation.h"

void preprocessing(Matrix *image);
void recognize(Page *page, Network *network);
void ocr(Network *network, char* image_path);

#endif