#include <stdio.h>
#include <ctype.h>
#include "segmentation/segmentation.h"
#include "neuralnet/network.h"
#include "neuralnet/output.h"
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

void postprocessing(char *post_process) {
    FILE *f_read;
    FILE *f_save;
    f_read = fopen(post_process, "r");
    f_save = fopen("post_processed.txt", "w");
    fseek(f_read, 0, SEEK_END);
    long size = ftell(f_read);
    fseek(f_read, 0, SEEK_SET);
    printf("size = %ld\n", size);
    char str[size];

    for (long i = 0; i < size; i++)
        str[i] = fgetc(f_read);

    char s = ' ';
    size_t dot_distance = 0;

    for (long i = 0; i < size; i++) {
        s = str[i];
        if (!dot_distance && s != ' ' && s != '\n' && s != '\t') {
            s = toupper(s);
            dot_distance = 1;
        }
        if (s == '.') {
            dot_distance = 0;
        }
        fwrite(&s, sizeof(char), 1, f_save);
    }
    fclose(f_read);
    fclose(f_save);
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
    preprocessing(&image);
    // Segmentation
    segment(image, &page);
    // Recognition
    recognize(page, network);
    // Export
    // output_save_default(page, "post.txt");
    // output_save_multi_column(page, "outmc.txt");

    postprocessing("post.txt"); //, "postprocessed.txt");
}