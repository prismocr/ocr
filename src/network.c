#include <stdio.h>
#include <assert.h>
#include <errno.h>
#include "network.h"
#include "layer.h"
#include "neuron.h"
#include "data.h"
#include "vector.h"

#define UNUSED(x) (void)(x)

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
    network->layers[0] = layer_new(sizes[0], NULL, &network->layers[1]);

    // Initializing Hidden Layers
    Layer *prev_layer;
    Layer *next_layer;
    for (size_t i = 1; i < nb_layers - 1; ++i) {
        prev_layer = &network->layers[i - 1];
        next_layer = &network->layers[i + 1];
        network->layers[i] = layer_new(sizes[i], prev_layer, next_layer);
    }

    // Initializing Output Layer
    network->layers[nb_layers - 1]
      = layer_new(sizes[nb_layers - 1], &network->layers[nb_layers - 2], NULL);
}

float *network_feed_forward(Network *network, float *input) {
    Layer *in_layer = network->input_layer;
    vector_copy(in_layer->nb_neurons, input, in_layer->values);
    for (size_t i = 1; i < network->nb_layers; i++) {
        layer_front_pop(&(network->layers[i]));
    }
    Layer *out_layer = network->output_layer;
    return out_layer->values;
}

// Network stochastic gradient descent
void network_sgd(Network *network, Dataset *dataset, size_t epochs,
                 size_t batch_size, float learning_rate) {
    Dataset *batches = NULL;
    size_t nb_batches = dataset->size / batch_size;
    // Main training loop
    for (size_t i = 1; i <= epochs; i++) {
        dataset_shuffle(dataset);
        // Divide dataset into several smaller batches of same size
        batches = initialize_batches(dataset, batch_size);
        // Train the network on each batch
        for (size_t j = 0; j < nb_batches; j++) {
            // Apply gradient descent on a whole batch
            batch_gd(network, &batches[j]);
            
            // Update network's weights and biases
            apply_grad(network, batches[j].size, learning_rate);
        }
    }

    network_print_results(*network, *dataset);
    free(batches);
}

// Apply gradient descent on a whole batch
void batch_gd(Network *network, Dataset *batch) {
    for (size_t i = 0; i < batch->size; i++) {
        network_backpropagation(network, batch->datas[i]);
    }
}

// Feed forward and backpropagate with a single @data
void network_backpropagation(Network *network, Data data) {
    // Forward Propagation
    float *output = network_feed_forward(network, data.input.val);

    // Backward Propagation
    Layer *out_l = network->output_layer;

    // Apply backpropagation on the last layer
    init_cost(out_l, output, data.target.val);
    vector_add(out_l->nb_neurons, out_l->deltas, out_l->d_biases);
    matrix_dcl(out_l->nb_neurons, out_l->deltas, out_l->prev_layer->nb_neurons,
               out_l->prev_layer->values, &out_l->d_weights);

    // Apply backpropagation on the remaining layers
    for (size_t i = network->nb_layers - 2; i > 0; i--) {
        layer_backpropagation(&network->layers[i]);
    }
}

// Update network's weights and biases
void apply_grad(Network *network, size_t size_batch, float learning_rate) {
    size_t i, j, k;
    for (i = 1; i < network->nb_layers; i++) {
        Layer *layer = &network->layers[i];
        for (j = 0; j < layer->nb_neurons; j++) {
            layer->biases[j]
              -= (learning_rate / size_batch) * layer->d_biases[j];
            layer->d_biases[j] = 0.f;
            for (k = 0; k < layer->prev_layer->nb_neurons; k++) {
                layer->weights.val[j][k]
                  -= (learning_rate / size_batch) * layer->d_weights.val[j][k];
                layer->d_weights.val[j][k] = 0.f;
            }
        }
    }
}

// Initialize cost of last layer
void init_cost(Layer *out_layer, float *output, float *target) {
    for (size_t i = 0; i < out_layer->nb_neurons; i++) {
        out_layer->deltas[i] = 2.f * (output[i] - target[i]); // derivative cost
        out_layer->deltas[i] *= sigmoid_prime(out_layer->z[i]);
    }
}

void network_print_clean(Network network) {
    printf("\n===================================================\n");
    for (size_t i = 1; i < network.nb_layers; i++) {
        printf("\n--------------------\nLayer %ld:\nWeights:\n", i);
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
        matrix_print(network.layers[i].weights);
        printf("Biases:\n");
        for (size_t j = 0; j < network.layers[i].nb_neurons; j++)
            printf("%f ", network.layers[i].biases[j]);
        printf("\nZ:\n");
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
        printf("\n");
    }
}

void network_print_results(Network network, Dataset dataset) {
    for (size_t i = 0; i < dataset.size; i++) {
        printf("Input data : ");
        vector_printf("%.3f ", dataset.datas[i].input);
        printf("\tOutput data : ");
        float *output
          = network_feed_forward(&network, dataset.datas[i].input.val);
        vector_printf("%.3f ",
                      arr2vect(output, network.output_layer->nb_neurons));
        printf("\n");
    }
}

void network_save(const char *path, Network network) {
    FILE *f;
    f = fopen(path, "wb");
    fputc(network.nb_layers, f);

    for (size_t i = 0; i < network.nb_layers; i++) {
        fputc(network.layers[i].nb_neurons, f);
    }
    for (size_t i = 1; i < network.nb_layers; i++) {
        for (size_t j = 0; j < network.layers[i].nb_neurons; j++) {
            for (size_t k = 0; k < network.layers[i-1].nb_neurons; k++) {
                fwrite(&network.layers[i].neurons[j].weights_in[k], 1,
                       sizeof(float), f);
            }
            fwrite(network.layers[i].neurons[j].bias, 1, sizeof(float), f);
        }
    }
    fclose(f);
}

int network_load(const char *path, Network *out) {
    FILE *f;
    f = fopen(path, "rb");
    if (f == NULL) {
        //set_last_errorf("Failed to open file: %s", strerror(errno));
        return 1;
    }

    size_t nb_layers = fgetc(f);
    size_t *sizes = (size_t *) malloc(nb_layers * sizeof(size_t));
    for (size_t i = 0; i < nb_layers; i++) {
        sizes[i] = fgetc(f);
    }
    Network network = network_new(nb_layers, sizes);

    for (size_t i = 1; i < network.nb_layers; i++) {
        for (size_t j = 0; j < network.layers[i].nb_neurons; j++) {
            for (size_t k = 0; k < network.layers[i-1].nb_neurons; k++) {
                size_t unused = fread(network.layers[i].neurons[j].weights_in + k, 1,
                      sizeof(float), f);
                UNUSED(unused);
            }
            size_t unused = fread(network.layers[i].neurons[j].bias, 1, sizeof(float), f);
            UNUSED(unused);
        }
    }

    *out = network;

    fclose(f);
    return 0;
}
