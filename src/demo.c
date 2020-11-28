#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "demo.h"
#include "segmentation/segmentation.h"
#include "utils/bitmap.h"
#include "imgproc/image.h"
#include "utils/error.h"
#include "utils/matrix.h"
#include "neuralnet/network.h"
#include "neuralnet/layer.h"

void sharpen_demo(int argc, char *argv[]) {
    Matrix image;

    if (argc < 3) {
        printf("Missing image path.\n");
        return;
    }

    exit_on_error(bitmap_load(argv[2], &image));

    sharpen(&image);

    exit_on_error(bitmap_save("out.bmp", &image));

    matrix_free(&image);
}

void blur_demo(int argc, char *argv[]) {
    Matrix image;

    if (argc < 3) {
        printf("Missing image path.\n");
        return;
    }

    exit_on_error(bitmap_load(argv[2], &image));

    wide_gauss(&image);

    exit_on_error(bitmap_save("out.bmp", &image));

    matrix_free(&image);
}

void rotate_demo(int argc, char *argv[]) {
    Matrix image;

    if (argc < 3) {
        printf("Missing image path.\n");
        return;
    }

    exit_on_error(bitmap_load(argv[2], &image));

    double angle = strtod(argv[3], NULL);
    image_rotate(&image, deg_to_rad(angle));

    exit_on_error(bitmap_save("out.bmp", &image));

    matrix_free(&image);
}

void contrast_demo(int argc, char *argv[]) {
    Matrix image;

    if (argc < 3) {
        printf("Missing image path.\n");
        return;
    }

    exit_on_error(bitmap_load(argv[2], &image));

    double delta = strtod(argv[3], NULL);
    image_contrast(&image, delta);

    exit_on_error(bitmap_save("out.bmp", &image));

    matrix_free(&image);
}

void edge_detect_demo(int argc, char *argv[]) {
    Matrix image;

    if (argc < 3) {
        printf("Missing image path.\n");
        return;
    }

    exit_on_error(bitmap_load(argv[2], &image));

    edge_detect(&image);

    exit_on_error(bitmap_save("out.bmp", &image));

    matrix_free(&image);
}

void invert_demo(int argc, char *argv[]) {
    Matrix image;

    if (argc < 3) {
        printf("Missing image path.\n");
        return;
    }

    exit_on_error(bitmap_load(argv[2], &image));

    image_invert_color(255.f, &image);

    exit_on_error(bitmap_save("out.bmp", &image));

    matrix_free(&image);
}

void network_demo(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Missing network training option.\n");
        printf("Use 'xor' or 'add' as option.\n");
        return;
    }
    srand(time(NULL));

    Vector vect000, vect001, vect010, vect011, vect100, vect101, vect110,
      vect111, vect00, vect01_1, vect01_2, vect01_3, vect10_1, vect10_2,
      vect10_3, vect11, vect0_1, vect0_2, vect1_1, vect1_2;
    vector_new(3, &vect000);
    vector_copy(3, (float[]){0, 0, 0}, vect000.val);
    vector_new(3, &vect001);
    vector_copy(3, (float[]){0, 0, 1}, vect001.val);
    vector_new(3, &vect010);
    vector_copy(3, (float[]){0, 1, 0}, vect010.val);
    vector_new(3, &vect011);
    vector_copy(3, (float[]){0, 1, 1}, vect011.val);
    vector_new(3, &vect100);
    vector_copy(3, (float[]){1, 0, 0}, vect100.val);
    vector_new(3, &vect101);
    vector_copy(3, (float[]){1, 0, 1}, vect101.val);
    vector_new(3, &vect110);
    vector_copy(3, (float[]){1, 1, 0}, vect110.val);
    vector_new(3, &vect111);
    vector_copy(3, (float[]){1, 1, 1}, vect111.val);

    vector_new(2, &vect00);
    vector_copy(2, (float[]){0, 0}, vect00.val);
    vector_new(2, &vect01_1);
    vector_copy(2, (float[]){0, 1}, vect01_1.val);
    vector_new(2, &vect01_2);
    vector_copy(2, (float[]){0, 1}, vect01_2.val);
    vector_new(2, &vect01_3);
    vector_copy(2, (float[]){0, 1}, vect01_3.val);
    vector_new(2, &vect10_1);
    vector_copy(2, (float[]){1, 0}, vect10_1.val);
    vector_new(2, &vect10_2);
    vector_copy(2, (float[]){1, 0}, vect10_2.val);
    vector_new(2, &vect10_3);
    vector_copy(2, (float[]){1, 0}, vect10_3.val);
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
    data01.input = vect01_1;
    data01.target = vect1_1;
    data10.input = vect10_1;
    data10.target = vect1_2;
    data11.input = vect11;
    data11.target = vect0_2;

    Data data000, data001, data010, data011, data100, data101, data110, data111;
    data000.input = vect000;
    data000.target = vect00;
    data001.input = vect001;
    data001.target = vect01_1;
    data010.input = vect010;
    data010.target = vect01_2;
    data011.input = vect011;
    data011.target = vect10_1;
    data100.input = vect100;
    data100.target = vect01_3;
    data101.input = vect101;
    data101.target = vect10_2;
    data110.input = vect110;
    data110.target = vect10_3;
    data111.input = vect111;
    data111.target = vect11;

    Dataset dataset;
    Network network;
    // create dim array to feed to network_new()
    size_t nb_layers = 3;

    if (!strcmp("xor", argv[2])) {
        // --- Create dataset---
        dataset_new(&dataset, 4);

        dataset.datas[0] = data00;
        dataset.datas[1] = data01;
        dataset.datas[2] = data10;
        dataset.datas[3] = data11;

        size_t sizes_static[] = {2, 2, 1};
        network = network_new(nb_layers, sizes_static);
        network_sgd(&network, &dataset, 100000, 4, 1.f, &dataset, 1);
    } else if (!strcmp("add", argv[2])) {
        // --- Create dataset---
        dataset_new(&dataset, 8);

        dataset.datas[0] = data000;
        dataset.datas[1] = data001;
        dataset.datas[2] = data010;
        dataset.datas[3] = data011;
        dataset.datas[4] = data100;
        dataset.datas[5] = data101;
        dataset.datas[6] = data110;
        dataset.datas[7] = data111;

        size_t sizes_static[] = {3, 3, 2};
        network = network_new(nb_layers, sizes_static);
        network_sgd(&network, &dataset, 1000, 4, 12.f, &dataset, 1);
    }
    network_print_results(network, dataset);

    network_print_clean(network);
    network_save("net.hex", &network);
    network_free(&network);

    dataset_free(&dataset);
}

void network_load_demo() {
    // create new network
    Network network;

    // load network
    network_load("net.hex", &network);

    // print network
    network_print_clean(network);
}

int otsu_demo(int argc, char *argv[]) {
    Matrix image;

    if (argc < 3) {
        printf("Missing image path.\n");
        return 1;
    }

    exit_on_error(bitmap_load(argv[2], &image));

    image_threshold_otsu(&image);

    exit_on_error(bitmap_save("out.bmp", &image));

    matrix_free(&image);
    return 0;
}

int segmentation_demo(int argc, char *argv[]) {
    Matrix image;

    if (argc < 3) {
        printf("Missing image path.\n");
        return 1;
    }

    exit_on_error(bitmap_load(argv[2], &image));

    segment_rlsa(image);

    matrix_free(&image);
    return 0;
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
        network_demo(argc, argv);
        return 0;
    }

    if (!strcmp(c, "network_load")) {
        network_load_demo();
        return 0;
    }

    if (!strcmp(c, "otsu")) {
        return otsu_demo(argc, argv);
    }

    if (!strcmp(c, "segmentation")) {
        return segmentation_demo(argc, argv);
    }

    printf("what?\n");

    return 1;
}
