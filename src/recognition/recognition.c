#include <stdio.h>
#include "segmentation/segmentation.h"
#include "neuralnet/network.h"
#include "neuralnet/output.h"
#include "recognition/recognition.h"
#include "utils/matrix.h"
#include "imgproc/image.h"
#include "imgproc/rotation.h"
#include "utils/bitmap.h"
#include "utils/error.h"
#include "textproc/text.h"

char network_get_result(Network *network, Matrix *image) {
    float input[image->w * image->w];
    matrix_linearization(image, input);
    return output_to_char(network_feed_forward(network, input));
}

void recognize_word(MatrixNode *image, char *letters, Network *network) {
    for (; image; image = image->next) {
        *letters = network_get_result(network, &image->val);
        printf("%c", *letters);
        letters++;
    }
}

void recognize_line(Word *word, Network *network) {
    for (; word; word = word->next) {
        // printf("x:%ld y:%ld w:%ld h:%ld length:%ld\n",word->x, word->y,
        // word->w, word->h, word->length);
        word->length = word->images.length;
        word->letters = malloc(word->length);
        if (word->length)
            recognize_word(word->images.first, word->letters, network);
        printf(" ");
    }
}

void recognize_region(Line *line, Network *network) {
    for (; line; line = line->next) {
        recognize_line(line->words, network);
        printf("\n");
    }
}

void recognize_page(Region *region, Network *network) {
    for (; region; region = region->next) {
        recognize_region(region->lines, network);
        printf("\n");
    }
}

void recognize(Page *page, Network *network) {
    for (; page; page = page->next) {
        recognize_page(page->regions, network);
        printf("\n=======================================\n");
    }
}

void preprocessing(Matrix *image) {
    sharpen(image);
    image_contrast(image, 20.f);
    image_auto_rotate(image, 0.01f);
}

void ocr(Network *network, char *image_path) {
    Page *page = NULL;
    Matrix image;
    exit_on_error(bitmap_load(image_path, &image));

    // Pre-process
    printf("Preprocessing...\n");
    preprocessing(&image);
    // Segmentation
    printf("Segmentation...\n");
    segment(image, &page);
    // Recognition
    printf("Recognizing...\n");
    recognize(page, network);
    // Post processing
    printf("Post processing...\n");
    post_process_words("./res/dictionaries/words_en.txt", page);
    // Export
    printf("Saving...\n");
    output_save_corrector(page, "corrector.txt");
    output_save_default(page, "post.txt");
    output_save_multi_column(page, "outmc.txt");
}