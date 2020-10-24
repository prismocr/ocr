#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "demo.h"
#include "segmentation.h"
#include "bitmap.h"
#include "image.h"
#include "error.h"
#include "matrix.h"
#include "network.h"
#include "layer.h"

void sharpen_demo(int argc, char *argv[]) {
    Matrix image;

    if (argc < 3) {
        printf("Missing image path.\n");
        return;
    }

    try
        (bitmap_load(argv[2], &image));

    sharpen(&image);

    try
        (bitmap_save("out.bmp", &image));

    matrix_free(&image);
}

void blur_demo(int argc, char *argv[]) {
    Matrix image;

    if (argc < 3) {
        printf("Missing image path.\n");
        return;
    }

    try
        (bitmap_load(argv[2], &image));

    wide_gauss(&image);

    try
        (bitmap_save("out.bmp", &image));

    matrix_free(&image);
}

void rotate_demo(int argc, char *argv[]) {
    Matrix image;

    if (argc < 3) {
        printf("Missing image path.\n");
        return;
    }

    try
        (bitmap_load(argv[2], &image));

    double angle = strtod(argv[3], NULL);
    image_rotate(&image, angle);

    try
        (bitmap_save("out.bmp", &image));

    matrix_free(&image);
}

void contrast_demo(int argc, char *argv[]) {
    Matrix image;

    if (argc < 2) {
        printf("Missing image path.\n");
        return;
    }

    try
        (bitmap_load(argv[2], &image));

    double delta = strtod(argv[3], NULL);
    image_contrast(&image, delta);

    try
        (bitmap_save("out.bmp", &image));

    matrix_free(&image);
}

void edge_detect_demo(int argc, char *argv[]) {
    Matrix image;

    if (argc < 3) {
        printf("Missing image path.\n");
        return;
    }

    try
        (bitmap_load(argv[2], &image));

    edge_detect(&image);

    try
        (bitmap_save("out.bmp", &image));

    matrix_free(&image);
}

void invert_demo(int argc, char *argv[]) {
    Matrix image;

    if (argc < 2) {
        printf("Missing image path.\n");
        return;
    }

    try
        (bitmap_load(argv[2], &image));

    image_invert_color(255.f, &image);

    try
        (bitmap_save("out.bmp", &image));

    matrix_free(&image);
}

void network_demo() {
    srand(time(NULL));
    // create dim array to feed to network_new()
    size_t nb_layers = 3;
    size_t sizes_static[] = {2, 2, 1};

    // create new network
    Network network = network_new(nb_layers, sizes_static);

    // --- Create dataset---
    Dataset dataset;
    dataset_new(&dataset, 4);

    Vector vect00, vect01, vect10, vect11, vect0_1, vect0_2, vect1_1, vect1_2;
    vector_new(2, &vect00);
    vector_copy(2, (float[]){0, 0}, vect00.val);
    vector_new(2, &vect01);
    vector_copy(2, (float[]){0, 1}, vect01.val);
    vector_new(2, &vect10);
    vector_copy(2, (float[]){1, 0}, vect10.val);
    vector_new(2, &vect11);
    vector_copy(2, (float[]){1, 1}, vect11.val);
    vector_new(1, &vect0_1);
    vector_copy(1, (float[]){0}, vect0_1.val);
    vector_new(1, &vect1_1);
    vector_copy(1, (float[]){1}, vect1_1.val);
    vector_new(1, &vect0_2);
    vector_copy(1, (float[]){0}, vect0_2.val);
    vector_new(1, &vect1_2);
    vector_copy(1, (float[]){1}, vect1_2.val);

    Data data00, data01, data10, data11;
    data00.input = vect00;
    data00.target = vect0_1;
    data01.input = vect01;
    data01.target = vect1_1;
    data10.input = vect10;
    data10.target = vect1_2;
    data11.input = vect11;
    data11.target = vect0_2;

    dataset.datas[0] = data00;
    dataset.datas[1] = data01;
    dataset.datas[2] = data10;
    dataset.datas[3] = data11;

    network_sgd(&network, &dataset, 1000000, 4, 2.5f);
    network_print_results(network, dataset);

    dataset_free(&dataset);

    network_print_clean(network);
    network_save("net.hex", network);
    network_free(&network);
}

void segmentation_demo(int argc, char *argv[]) {
    Matrix image;

    if (argc < 3) {
        printf("Missing image path.\n");
        return;
    }

    try
        (bitmap_load(argv[2], &image));

    segment_morph_hist(image);

    matrix_free(&image);
}

void network_load_demo() {
    // create new network
    Network network;

    // load network
    network_load("net.hex", &network);

    // print network
    network_print_clean(network);
}

int demo(int argc, char *argv[]) {
    char *c = argv[1];

    if (argc < 2) {
        printf("Please enter a valid command.\n");
        return 1;
    }

    if (!strcmp(c, "sharpen")) {
        sharpen_demo(argc, argv);
        return 0;
    }

    if (!strcmp(c, "blur")) {
        blur_demo(argc, argv);
        return 0;
    }

    if (!strcmp(c, "rotate")) {
        rotate_demo(argc, argv);
        return 0;
    }

    if (!strcmp(c, "edge_detect")) {
        edge_detect_demo(argc, argv);
        return 0;
    }

    if (!strcmp(c, "contrast")) {
        contrast_demo(argc, argv);
        return 0;
    }

    if (!strcmp(c, "invert")) {
        invert_demo(argc, argv);
        return 0;
    }

    if (!strcmp(c, "network")) {
        network_demo();
        return 0;
    }

    if (!strcmp(c, "segmentation")) {
        segmentation_demo(argc, argv);
        return 0;
    }

    if (!strcmp(c, "network_load")) {
        network_load_demo();
        return 0;
    }

    printf("what?\n");

    return 1;
}
