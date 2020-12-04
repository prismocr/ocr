#include <stdio.h>
#include "segmentation/segmentation.h"
#include "neuralnet/network.h"
#include "recognition/recognition.h"
#include "utils/matrix.h"
#include "imgproc/image.h"
#include "imgproc/rotation.h"

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

void recognize_document(Page *page, Network *network) {
    while (page) {
        recognize_page(page->regions, network);
        printf("\n=======================================\n");
        page = page->next;
    }
}

void recognize(Matrix *image, Network *net) {
    // Pre-process
    sharpen(image);
    image_contrast(image, 20.f);
    image_auto_rotate(image, 0.01f);

    // Segmentation
    Page *page;
    segment(*image, &page);

    recognize_document(page, net);
}