#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "neuralnet/model.h"
#include "neuralnet/data.h"
#include "utils/matrix.h"
#include "utils/vector.h"
#include "utils/error.h"
#include "utils/bitmap.h"

#define LOWER_LETTER_ORG 0
#define UPPER_LETTER_ORG 26
#define DIGIT_ORG 52
#define SPECIAL_CHAR_ORG 62

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

char output_to_char(float *output) {
    size_t max = 0;
    for (size_t i = 0; i < OUTPUT_SIZE; i++) {
        if (output[i] > output[max]) {
            max = i;
        }
    }

    if (max < UPPER_LETTER_ORG)
        return max + 'a';
    if (max < DIGIT_ORG)
        return max - UPPER_LETTER_ORG + 'A';
    if (max < SPECIAL_CHAR_ORG)
        return max - DIGIT_ORG + '0';
    switch (max - SPECIAL_CHAR_ORG) {
        case 0:
            return '!';
        case 1:
            return '\"';
        case 2:
            return '\'';
        case 3:
            return '(';
        case 4:
            return ')';
        case 5:
            return ',';
        case 6:
            return '-';
        case 7:
            return '.';
        case 8:
            return ':';
        case 9:
            return ';';
        case 10:
            return '?';
    }
    return '#';
}

void dataset_double_capacity(Dataset *dataset) {
    dataset->size *= 2;
    dataset->datas = realloc(dataset->datas, dataset->size * sizeof(Data));
}

void data_init_input(char *image_path, Vector *input) {
    Matrix image;
    exit_on_error(bitmap_load(image_path, &image));
    matrix_scale(&image, 0.99f / 255);
    matrix_add_const(&image, 0.01f);

    input->size = IMAGE_WIDTH * IMAGE_WIDTH;
    input->val = calloc(input->size, sizeof(float));
    for (size_t i = 0; i < image.h; i++) {
        vector_copy(image.w, image.val[i], &(input->val[i * image.w]));
    }

    matrix_free(&image);
}

void data_init_target(char character, Vector *target) {
    target->size = OUTPUT_SIZE;
    target->val = calloc(OUTPUT_SIZE, sizeof(float));

    if (character >= 'a' && character <= 'z') {
        target->val[character - 'a' + LOWER_LETTER_ORG] = 1.f;
    } else if (character >= 'A' && character <= 'Z') {
        target->val[character - 'A' + UPPER_LETTER_ORG] = 1.f;
    } else if (character >= '0' && character <= '9') {
        target->val[character - '0' + DIGIT_ORG] = 1.f;
    } else {
        switch (character) {
            case '!':
                target->val[SPECIAL_CHAR_ORG + 0] = 1.f;
                break;
            case '\"':
                target->val[SPECIAL_CHAR_ORG + 1] = 1.f;
                break;
            case '\'':
                target->val[SPECIAL_CHAR_ORG + 2] = 1.f;
                break;
            case '(':
                target->val[SPECIAL_CHAR_ORG + 3] = 1.f;
                break;
            case ')':
                target->val[SPECIAL_CHAR_ORG + 4] = 1.f;
                break;
            case ',':
                target->val[SPECIAL_CHAR_ORG + 5] = 1.f;
                break;
            case '-':
                target->val[SPECIAL_CHAR_ORG + 6] = 1.f;
                break;
            case '.':
                target->val[SPECIAL_CHAR_ORG + 7] = 1.f;
                break;
            case ':':
                target->val[SPECIAL_CHAR_ORG + 8] = 1.f;
                break;
            case ';':
                target->val[SPECIAL_CHAR_ORG + 9] = 1.f;
                break;
            case '?':
                target->val[SPECIAL_CHAR_ORG + 10] = 1.f;
                break;
        }
    }
}

char *strdup(const char *str) {
    int n = strlen(str) + 1;
    char *dup = malloc(n);
    if (dup) {
        strcpy(dup, str);
    }
    return dup;
}

void load_dataset(char directory_path[], Dataset *dataset) {
    char csv_file_path[1024], images_directory_path[1024];
    strcpy(csv_file_path, directory_path);
    strcat(csv_file_path, "data.csv");
    strcpy(images_directory_path, directory_path);
    strcat(images_directory_path, "images/");

    FILE *csv_file = fopen(csv_file_path, "r");
    if (csv_file == NULL) {
        printf("\033[41m Incorrect dataset path \033[0m\n");
        exit(1);
    }

    dataset_new(dataset, 1);

    size_t i = 0; // index within dataset's datas array
    char line[1024], *tmp, *label, *image_name, image_path[1024];
    while (fgets(line, 1024, csv_file)) {
        // tmp = strdup(line);
        label = strtok(line, "§");
        if (!label) {
            label = ",";
            tmp += 2;
        }
        image_name = strtok(NULL, "§\n");
        strcpy(image_path, images_directory_path);
        strcat(image_path, image_name);

        if (i == dataset->size) {
            dataset_double_capacity(dataset);
        }
        data_init_input(image_path, &dataset->datas[i].input);
        data_init_target(label[0], &dataset->datas[i].target);

        // free(tmp);
        i++;
    }

    fclose(csv_file);
    dataset->size = i;
    dataset->datas = realloc(dataset->datas, dataset->size * sizeof(Data));
}