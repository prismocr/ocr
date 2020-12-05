#include <stdio.h>
#include "segmentation/segmentation.h"
#include "neuralnet/network.h"
#include "recognition/recognition.h"
#include "utils/matrix.h"
#include "imgproc/image.h"
#include "imgproc/rotation.h"
#include "utils/bitmap.h"
#include "utils/error.h"

char network_get_result(Network *network, Matrix *image) {
    float input[image->w * image->w];
    matrix_linearization(image, input);
    return output_to_char(network_feed_forward(network, input));
}

void recognize_word(MatrixNode *image, char *letters, Network *network) {
    while (image) {
        *letters = network_get_result(network, &image->val);
        printf("%c", *letters);
        image = image->next;
        letters++;
    }
}

void recognize_line(Word *word, Network *network) {
    while (word) {
        //printf("x:%ld y:%ld w:%ld h:%ld length:%ld\n",word->x, word->y, word->w, word->h, word->length);
        word->length = word->images.length;
        word->letters = malloc(word->length);
        if(word->length)
            recognize_word(word->images.first, word->letters, network);
        printf(" ");
        word = word->next;
    }
}

void recognize_region(Line *line, Network *network) {
    while (line) {
        recognize_line(line->words, network);
        printf("\n");
        line = line->next;
    }
}

void recognize_page(Region *region, Network *network) {
    while (region) {
        recognize_region(region->lines, network);
        printf("\n");
        region = region->next;
    }
}

void recognize(Page *page, Network *network) {
    while (page) {
        recognize_page(page->regions, network);
        printf("\n=======================================\n");
        page = page->next;
    }
}

void preprocessing(Matrix *image){
    sharpen(image);
    image_contrast(image, 20.f);
    image_auto_rotate(image, 0.01f);
}

void ocr(Network *network, char* image_path){
    Page *page = NULL;
    Matrix image;
    exit_on_error(bitmap_load(image_path, &image));

    // Pre-process
    preprocessing(&image);
    // Segmentation
    segment(image, &page);
    // Recognition
    recognize(page, network);
    // Export
    // Brice stuff
}