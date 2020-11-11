#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "neuralnet/data.h"
#include "utils/vector.h"

int dataset_new(Dataset *dataset, size_t size) {
    assert(size > 0);
    dataset->size = size;
    dataset->datas = (Data *) calloc(size, sizeof(Data));
    return dataset->datas == NULL;
}

void dataset_free(Dataset *dataset) {
    for (size_t i = 0; i < dataset->size; i++) {
        data_free(&dataset->datas[i]);
    }
    free(dataset->datas);
}

void data_free(Data *data) {
    vector_free(&data->input);
    vector_free(&data->target);
}

void dataset_shuffle(Dataset *dataset) {
    for (int i = dataset->size - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        data_swap(&dataset->datas[i], &dataset->datas[j]);
    }
}

void data_swap(Data *a, Data *b) {
    Data temp = *a;
    *a = *b;
    *b = temp;
}

Dataset *initialize_batches(Dataset *dataset, size_t batch_size) {
    size_t nb_batches = dataset->size / batch_size;
    Dataset *batches = calloc(nb_batches, sizeof(Dataset));
    assert(batches != NULL);

    for (size_t i = 0; i < nb_batches; i++) {
        dataset_new(&batches[i], batch_size);
        for (size_t j = 0; j < batch_size; j++) {
            batches[i].datas[j] = dataset->datas[i * batch_size + j];
        }
    }

    return batches;
}
