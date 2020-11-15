#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "neuralnet/data.h"
#include "utils/matrix.h"
#include "utils/vector.h"
#include "utils/error.h"
#include "utils/bitmap.h"

#define OUTPUT_SIZE 52
#define LOWER_LETTER_ORG 1
#define UPPER_LETTER_ORG 27
//#define DIGIT_ORG 53

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

void dataset_double_capacity(Dataset *dataset){
    dataset->size *= 2;
    dataset->datas = realloc(dataset->datas, dataset->size * sizeof(Data));
}

void data_init_input(char *image_path, Vector *input){
    Matrix image;
    exit_on_error(bitmap_load(image_path, &image));

    for(size_t i=0; i<image.h; i++){
        vector_copy(image.w, image.val[i], &(input->val[i*image.w]));
    }

    matrix_free(&image);
}

void data_init_target(char character, Vector *target){
    target->size = OUTPUT_SIZE;
    target->val = (float*) calloc(OUTPUT_SIZE, sizeof(float));

    if(character >= 'a' && character <= 'z'){
        target->val[character-'a'+LOWER_LETTER_ORG] = 1.f;
    }
    else if(character >= 'A' && character <= 'Z'){
        target->val[character-'A'+UPPER_LETTER_ORG] = 1.f;
    }
}

char *strdup(const char *str)
{
    int n = strlen(str) + 1;
    char *dup = malloc(n);
    if(dup)
    {
        strcpy(dup, str);
    }
    return dup;
}

void generate_dataset(char directory_path[], Dataset *dataset){
    char csv_file_path[1024], images_directory_path[1024];
    strcpy(csv_file_path,directory_path);
    strcat(csv_file_path, "data.csv");
    strcpy(images_directory_path,directory_path);
    strcat(images_directory_path, "images/");

    FILE* csv_file = fopen(csv_file_path,"r");
    if(csv_file == NULL){
        printf("\033[41m Incorrect dataset path \033[0m\n");
        exit(1);
    }

    dataset_new(dataset,1);

    size_t i = 0; //index within dataset's datas array
    char line[1024], *tmp, *label, *image_name, image_path[1024];
    while(fgets(line,1024,csv_file)){
        tmp = strdup(line);
        
        label = strtok(line,",");
        image_name = strtok(NULL,",\n");
        strcpy(image_path,images_directory_path);
        strcat(image_path,image_name);

        if(i == dataset->size){
            dataset_double_capacity(dataset);
        }
        Data data;
        printf("%s\n", image_path);
        data_init_input(image_path,&data.target);
        printf("YOOOO_line_%ld\n",i);
        data_init_target(label[0],&data.target);
        
        free(tmp);
        i++;
    }

    dataset->size = i;
    dataset->datas = realloc(dataset->datas, dataset->size * sizeof(Data));
}