#ifndef RECOGNITION_H
#define RECOGNITION_H

#include "neuralnet/network.h"
#include "segmentation/segmentation.h"

void recognize_document(Page *page, Network *network);

#endif