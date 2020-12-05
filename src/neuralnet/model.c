#include <stdio.h>
#include <assert.h>
#include "neuralnet/model.h"

void model_new(N_cfg *cfg, Model *model) {
    assert(cfg->batch_size > 0 && cfg->eta > 0.f && cfg->epochs > 0);
    model->cfg = cfg;

    // size_t test_data_size = (long) NB_TEST_FONTS * OUTPUT_SIZE

    // Complete Dataset
    load_dataset(cfg->dataset_path, &model->dataset);
    size_t test_data_size = model->dataset.size * cfg->test_data_ratio;
    size_t training_data_size = model->dataset.size - test_data_size;

    // Training dataset and Validation dataset
    dataset_new(&model->test_data, test_data_size);
    dataset_new(&model->training_data, training_data_size);
    model->test_data.datas = model->dataset.datas;
    model->training_data.datas = &(model->dataset.datas[test_data_size]);

    model->network = network_new(cfg->nb_layers, cfg->layer_sizes);
}

void model_free(Model *model) {
    dataset_free(&model->dataset);
    network_free(&model->network);
}

void model_train(Model *model, int monitor) {
    network_sgd(&model->network, &model->training_data, model->cfg->epochs,
                model->cfg->batch_size, model->cfg->eta, &model->test_data,
                monitor);
}

float model_evaluate(Model *model) {
    size_t nb_correct_output
      = network_evaluate(&model->network, &model->dataset, 1);
    float percentage = (float)nb_correct_output*100/model->dataset.size;
    printf("%ld/%ld : %f\n", nb_correct_output, model->dataset.size,percentage);
    return (float) percentage;
}