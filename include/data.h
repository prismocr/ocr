#ifndef DATA_H
#define DATA_H

#include "vector.h"

typedef struct Data Data;
struct Data {
    Vector input;
    Vector target;
};

typedef struct Dataset Dataset;
struct Dataset {
    size_t size;
    Data *datas;
};

int dataset_new(Dataset *dataset, size_t size);
void dataset_free(Dataset *dataset);
void data_free(Data *data);

void dataset_shuffle(Dataset *dataset);
void data_swap(Data *a, Data *b);

Dataset *initialize_batches(Dataset *dataset, size_t batch_size);

#endif