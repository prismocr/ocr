#include <stdio.h>
#include <assert.h>
#include <errno.h>
#include <math.h>
#include "neuralnet/network.h"
#include "neuralnet/layer.h"
#include "neuralnet/neuron.h"
#include "neuralnet/data.h"
#include "utils/vector.h"

#define UNUSED(x) (void) (x)

Network network_new(size_t nb_layers, size_t *sizes) {
    Network network;

    network.nb_layers = nb_layers;
    initialize_layers(&network, nb_layers, sizes);
    network.input_layer = &(network.layers[0]);
    network.output_layer = &(network.layers[nb_layers - 1]);

    return network;
}

void network_free(Network *network) {
    for (size_t i = 0; i < network->nb_layers; ++i) {
        layer_free(&(network->layers[i]));
    }
    free(network->layers);
}

void initialize_layers(Network *network, size_t nb_layers, size_t *sizes) {
    network->layers = (Layer *) malloc(nb_layers * sizeof(Layer));

    // Initializing Input Layer
    network->layers[0]
      = layer_new(sizes[0], NULL, &network->layers[1], SIGMOID);

    // Initializing Hidden Layers
    Layer *prev_layer;
    Layer *next_layer;
    for (size_t i = 1; i < nb_layers - 1; ++i) {
        prev_layer = &network->layers[i - 1];
        next_layer = &network->layers[i + 1];
        network->layers[i]
          = layer_new(sizes[i], prev_layer, next_layer, SIGMOID);
    }

    // Initializing Output Layer
    network->layers[nb_layers - 1] = layer_new(
      sizes[nb_layers - 1], &network->layers[nb_layers - 2], NULL, SIGMOID);
}

float *network_feed_forward(Network *network, float *input) {
    Layer *in_layer = network->input_layer;
    vector_copy(in_layer->nb_neurons, input, in_layer->values);
    for (size_t i = 1; i < network->nb_layers; i++) {
        layer_front_pop(&(network->layers[i]));
    }

    return network->output_layer->values;
}

void network_sgd(Network *network, Dataset *dataset_training, size_t epochs,
                 size_t batch_size, float learning_rate, float momentum,
                 Dataset *dataset_evaluation, int monitor_training) {
    Dataset *batches = initialize_batches(dataset_training, batch_size);
    size_t nb_batches = dataset_training->size / batch_size, nb_correct_output;
    // Main training loop
    for (size_t i = 1; i <= epochs; i++) {
        dataset_shuffle(dataset_training);
        // Divide dataset into several smaller batches of same size
        batches = initialize_batches(dataset_training, batch_size);
        // Train the network on each batch
        for (size_t j = 0; j < nb_batches; j++) {
            // Apply gradient escent on a whole batch
            // batch_gd(network, &batches[j], learning_rate, lambda,
            // dataset_training->size);
            for (size_t i = 0; i < batch_size; i++) {
                network_backpropagation(network, batches[j].datas[i]);
            }
            // Update network's weights and biases
            apply_grad(network, batch_size, learning_rate, momentum);
        }
        printf("Epoch %ld complete\n", i);
        if (monitor_training && dataset_evaluation->size > 0) {
            nb_correct_output
              = network_evaluate(network, dataset_evaluation, 0);
            printf("Epoch %ld: %ld/%ld : %f\n", i, nb_correct_output,
                   dataset_evaluation->size,
                   (float) nb_correct_output * 100 / dataset_evaluation->size);
        }
    }

    free(batches);
}

/*void batch_gd(Network *network, Dataset *batch, float learning_rate, float
lambda, float dataset_size) { for (size_t i = 0; i < batch->size; i++) {
        network_backpropagation(network, batch->datas[i]);
    }
    // Update network's weights and biases
    apply_grad(network, batch->size, learning_rate, float lambda, );
}*/

void network_backpropagation(Network *network, Data data) {
    // Forward Propagation
    /*printf("size %ld\n",data.input.size);
    vector_print(arr2vect(data.input.val,data.input.size));
    printf("\n");*/
    network_feed_forward(network, data.input.val);

    // Backward Propagation
    Layer *out_l = network->output_layer;

    // Apply backpropagation on the last layer
    init_cost(out_l, data.target.val);
    vector_add(out_l->nb_neurons, out_l->deltas, out_l->d_biases);

    matrix_dcl(out_l->nb_neurons, out_l->deltas, out_l->prev_layer->nb_neurons,
               out_l->prev_layer->values, &out_l->d_weights);

    // Apply backpropagation on the remaining layers
    for (size_t i = network->nb_layers - 2; i > 0; i--) {
        layer_backpropagation(&network->layers[i]);
    }
}

void apply_grad(Network *network, size_t size_batch, float learning_rate,
                float momentum) {
    size_t i, j, k;
    float scale = learning_rate / size_batch;
    for (i = 1; i < network->nb_layers; i++) {
        Layer *layer = &network->layers[i];
        for (j = 0; j < layer->nb_neurons; j++) {
            layer->v_biases[j]
              = momentum * layer->v_biases[j] - scale * layer->d_biases[j];
            layer->biases[j] += layer->v_biases[j];
            layer->d_biases[j] = 0.f;
            for (k = 0; k < layer->prev_layer->nb_neurons; k++) {
                layer->v_weights.val[j][k]
                  = momentum * layer->v_weights.val[j][k]
                    - scale * layer->d_weights.val[j][k];
                layer->weights.val[j][k] += layer->v_weights.val[j][k];
                // layer->weights.val[j][k] -= scale *
                // layer->d_weights.val[j][k];
                layer->d_weights.val[j][k] = 0.f;
            }
        }
    }
}

float quadratic_cost_delta(float z, float output, float target) {
    return output - target * sigmoid_prime(z);
}

float cross_entropy_cost_delta(float output, float target) {
    return output - target;
}

void init_cost(Layer *out_layer, float *target) {
    float *output = out_layer->values;
    for (size_t i = 0; i < out_layer->nb_neurons; i++) {
        out_layer->deltas[i] = cross_entropy_cost_delta(output[i], target[i]);
    }
}
void network_print_clean(Network network) {
    printf("\n===================================================\n");
    for (size_t i = 1; i < network.nb_layers; i++) {
        printf("\n--------------------\nLayer %zu:\nWeights:\n", i);
        matrix_print(network.layers[i].weights);
        printf("Biases:\n");
        for (size_t j = 0; j < network.layers[i].nb_neurons; j++)
            printf("%f ", network.layers[i].biases[j]);
        printf("\n");
    }
}

void network_print(Network network) {
    printf("\n===================================================\n");
    for (size_t i = 1; i < network.nb_layers; i++) {
        printf("\n--------------------\nLayer %ld:\nWeights:\n", i);
        matrix_printf("%.2f ", network.layers[i].weights);
        printf("Biases:\n");
        for (size_t j = 0; j < network.layers[i].nb_neurons; j++)
            printf("%.2f ", network.layers[i].biases[j]);

        /*printf("\nZ:\n");
        for (size_t j = 0; j < network.layers[i].nb_neurons; j++)
            printf("%f ", network.layers[i].z[j]);

        printf("\nValues:\n");
        for (size_t j = 0; j < network.layers[i].nb_neurons; j++)
            printf("%f ", network.layers[i].values[j]);

        printf("\n\nD_Weights:\n");
        matrix_print(network.layers[i].d_weights);

        printf("D_Biases:\n");
        for (size_t j = 0; j < network.layers[i].nb_neurons; j++)
            printf("%f ", network.layers[i].d_biases[j]);

        printf("\nDeltas:\n");
        for (size_t j = 0; j < network.layers[i].nb_neurons; j++)
            printf("%f ", network.layers[i].deltas[j]);
        printf("\n");*/
    }
}

void network_print_results(Network network, Dataset dataset) {
    for (size_t i = 0; i < dataset.size; i++) {
        printf("Input data : ");
        vector_printf("%.0f ", dataset.datas[i].input);
        printf("\tOutput data : ");
        float *output
          = network_feed_forward(&network, dataset.datas[i].input.val);
        vector_printf("%.0f ",
                      arr2vect(output, network.output_layer->nb_neurons));
        printf("\n");
    }
}

void network_save(const char *path, Network *network) {
    FILE *f;
    f = fopen(path, "wb");
    fputc(network->nb_layers, f);

    for (size_t i = 0; i < network->nb_layers; i++) {
        fwrite(&network->layers[i].nb_neurons, sizeof(size_t), 1, f);
    }
    for (size_t i = 1; i < network->nb_layers; i++) {
        for (size_t j = 0; j < network->layers[i].nb_neurons; j++) {
            for (size_t k = 0; k < network->layers[i - 1].nb_neurons; k++) {
                fwrite(&network->layers[i].neurons[j].weights_in[k], 1,
                       sizeof(float), f);
            }
            fwrite(network->layers[i].neurons[j].bias, 1, sizeof(float), f);
        }
    }
    fclose(f);
}

int network_load(const char *path, Network *out) {
    FILE *f;
    f = fopen(path, "rb");
    if (f == NULL) {
        // set_last_errorf("Failed to open file: %s", strerror(errno));
        return 1;
    }

    size_t nb_layers = fgetc(f);
    size_t *sizes = (size_t *) malloc(nb_layers * sizeof(size_t));
    for (size_t i = 0; i < nb_layers; i++) {
        size_t unused = fread(&sizes[i], sizeof(size_t), 1, f);
        UNUSED(unused);
        // printf("size layer %ld: %ld\n", i, sizes[i]);
    }
    Network network = network_new(nb_layers, sizes);

    for (size_t i = 1; i < network.nb_layers; i++) {
        for (size_t j = 0; j < network.layers[i].nb_neurons; j++) {
            for (size_t k = 0; k < network.layers[i - 1].nb_neurons; k++) {
                size_t unused
                  = fread(network.layers[i].neurons[j].weights_in + k, 1,
                          sizeof(float), f);
                UNUSED(unused);
            }
            size_t unused
              = fread(network.layers[i].neurons[j].bias, 1, sizeof(float), f);
            UNUSED(unused);
        }
    }

    *out = network;

    fclose(f);
    return 0;
}

size_t network_evaluate(Network *network, Dataset *test_data, int show_errors) {
    size_t nb_correct = 0;
    char target_char, output_char;
    for (size_t i = 0; i < test_data->size; i++) {
        target_char = output_to_char(test_data->datas[i].target.val);
        network_feed_forward(network, test_data->datas[i].input.val);
        output_char = output_to_char(network->output_layer->values);
        // printf("i:%ld target:%c ouput:%c\n",i,target_char,output_char);
        if (output_char == target_char) {
            nb_correct++;
        } else if (show_errors) {
            printf("i:%ld target:%c ouput:%c\n", i, target_char, output_char);
        }
    }
    return nb_correct;
}