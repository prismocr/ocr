#ifndef RECOGNITION_H
#define RECOGNITION_H

#include "neuralnet/network.h"
#include "segmentation/segmentation.h"
#include "textproc/dict.h"

Matrix preprocessing(Matrix *image);
void recognize(Page *page, Network *network);
void ocr(Network *network, char *image_path);
char network_get_result(Network *network, Matrix *image);

#endif