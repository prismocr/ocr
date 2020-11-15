#ifndef DATA_H
#define DATA_H

#include "utils/vector.h"

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

/**
 * Creates a new empty Dataset of size @size
 *
 * @param dataset to save the new dataset in
 * @param size of the new dataset
 * @return state (error)
 */
int dataset_new(Dataset *dataset, size_t size);
/**
 * Frees a dataset
 *
 * @param dataset to free
 */
void dataset_free(Dataset *dataset);
/**
 * Frees a data
 *
 * @param data to free
 */
void data_free(Data *data);

/**
 * Shuffles a dataset for stochastic gradient descent
 *
 * @param dataset dataset to shuffle
 */
void dataset_shuffle(Dataset *dataset);
/**
 * Swaps 2 data @param a and @param b
 *
 * @param data a
 * @param data b
 */
void data_swap(Data *a, Data *b);

/**
 * Divides a @param dataset into multiple batches of same size @param batch_size
 *
 * @param dataset to divide
 * @param size of the batches
 * @return array of datasets, each of size @param batch_size
 */
Dataset *initialize_batches(Dataset *dataset, size_t batch_size);

void dataset_double_capacity(Dataset *dataset);
void data_init_input(char *image_path, Vector *input);
void data_init_output(char character, Vector *output);
void generate_dataset(char directory_path[], Dataset *dataset);
#endif
