#ifndef MODEL_H
#define MODEL_H

#include "neuralnet/network.h"
#include "neuralnet/data.h"

#define OUTPUT_SIZE 47
//#define OUTPUT_SIZE 26
#define IMAGE_WIDTH 28

typedef struct Network_config N_cfg;
struct Network_config {
    size_t epochs;
    size_t batch_size;
    float eta;
    float lambda;
    float test_data_ratio;
    char *dataset_path;
    size_t nb_layers;
    size_t *layer_sizes;
};

typedef struct Model Model;
struct Model {
    Network network;
    Dataset dataset, training_data, test_data;
    N_cfg *cfg;
};

void model_new(N_cfg *cfg, Model *model);
void model_free(Model *model);
void model_train(Model *model, int monitor);
float model_evaluate(Model *model);

#endif