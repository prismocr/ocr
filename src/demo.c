#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "demo.h"
#include "segmentation/segmentation.h"
#include "utils/bitmap.h"
#include "imgproc/image.h"
#include "imgproc/rotation.h"
#include "utils/error.h"
#include "utils/matrix.h"
#include "neuralnet/network.h"
#include "neuralnet/layer.h"
#include "neuralnet/output.h"

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

    image_threshold_otsu(&image);
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

    Dataset dataset = {0};
    Network network = {0};
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

    Page *page = NULL;
    segment(image, &page);

    page_free(&page);
    matrix_free(&image);
    return 0;
}

int skew_detect_demo(int argc, char *argv[]) {
    Matrix image;

    if (argc < 3) {
        printf("Missing image path.\n");
        return 1;
    }

    exit_on_error(bitmap_load(argv[2], &image));
    float angle = image_detect_skew(&image, 0.01f);
    printf("Skew = %f deg\n", rad_to_deg(angle));

    matrix_free(&image);

    return 0;
}

int auto_rotate(int argc, char *argv[]) {
    Matrix image;

    if (argc < 3) {
        printf("Missing image path.\n");
        return 1;
    }

    exit_on_error(bitmap_load(argv[2], &image));
    image_auto_rotate(&image, 0.01f);

    exit_on_error(bitmap_save("out.bmp", &image));

    matrix_free(&image);

    return 0;
}

void process_word_demo(int argc, char *argv[]) {
    if (argc < 5) {
        printf("Missing images paths.\n");
        return;
    }
    Word w;
    w.x = 0;
    w.y = 0;
    w.length = 3;
    char letters[3];
    w.letters = letters;

    Matrix image1;
    Matrix image2;
    Matrix image3;

    exit_on_error(bitmap_load(argv[2], &image1));
    exit_on_error(bitmap_load(argv[3], &image2));
    exit_on_error(bitmap_load(argv[4], &image3));

    MatrixLinkedList mll;
    mll.length = 3;

    MatrixNode mn1;
    MatrixNode mn2;
    MatrixNode mn3;

    mll.first = &mn1;

    mn1.val = image1;
    mn2.val = image1;
    mn3.val = image1;

    mn1.next = &mn2;
    mn2.next = &mn3;
    mn2.next = NULL;

    w.images = mll;
    w.next = NULL;

    process_word(&w);

    for (size_t i = 0; i < w.length; i++) {
        printf("%c ", w.letters[i]);
    }
    printf("\n");

    return;
}

void save_pages_demo() {
    // PAGE 1
    Page p1;
    Region r11;
    Line l111;
    Word w1111;
    Word w1112;
    Word w1113;

    Line l112;
    Word w1121;

    Line l113;
    Word w1131;

    Region r12;
    Line l121;
    Word w1211;

    w1111.length = 3;
    w1112.length = 4;
    w1113.length = 7;
    w1121.length = 6;
    w1131.length = 3;
    w1211.length = 2;

    w1111.letters = "hey";
    w1112.letters = "haha";
    w1113.letters = "bonjour";
    w1121.letters = "coucou";
    w1131.letters = "ah!";
    w1211.letters = "hi";

    w1111.next = &w1112;
    w1112.next = &w1113;
    w1113.next = NULL;
    w1121.next = NULL;
    w1131.next = NULL;
    w1211.next = NULL;

    l111.next = &l112;
    l112.next = &l113;
    l113.next = NULL;
    l121.next = NULL;

    r11.next = &r12;
    r12.next = NULL;

    p1.regions = &r11;

    r11.lines = &l111;
    r12.lines = &l121;

    l111.words = &w1111;
    l112.words = &w1121;
    l113.words = &w1131;
    l121.words = &w1211;

    // PAGE 2
    Page p2;
    Region r21;
    Line l211;
    Word w2111;
    Word w2112;
    Word w2113;

    Line l212;
    Word w2121;

    Line l213;
    Word w2131;

    Region r22;
    Line l221;
    Word w2211;

    w2111.length = 3;
    w2112.length = 4;
    w2113.length = 7;
    w2121.length = 6;
    w2131.length = 3;
    w2211.length = 2;

    w2111.letters = "boo";
    w2112.letters = "adam";
    w2113.letters = "yolebro";
    w2121.letters = "cucucu";
    w2131.letters = "euh";
    w2211.letters = ":D";

    w2111.next = &w2112;
    w2112.next = &w2113;
    w2113.next = NULL;
    w2121.next = NULL;
    w2131.next = NULL;
    w2211.next = NULL;

    l211.next = &l212;
    l212.next = &l213;
    l213.next = NULL;
    l221.next = NULL;

    r21.next = &r22;
    r22.next = NULL;

    p2.regions = &r21;

    r21.lines = &l211;
    r22.lines = &l221;

    l211.words = &w2111;
    l212.words = &w2121;
    l213.words = &w2131;
    l221.words = &w2211;

    p1.next = &p2;
    p2.next = NULL;
    output_save_default(&p1, "testpages.txt");
}

void output_save_multi_column_demo() {
    // PAGE 1
    Page p;
    Region r1;
    Line l1;
    Line l2;
    Line l3;
    Line l4;

    Word w1;
    Word w2;
    Word w3;
    Word w4;
    Word w5;
    Word w6;
    Word w7;
    Word w8;

    w1.length = 1;
    w2.length = 2;
    w3.length = 3;
    w4.length = 4;
    w5.length = 5;
    w6.length = 6;
    w7.length = 7;
    w8.length = 8;

    w1.letters = "1";
    w2.letters = "22";
    w3.letters = "333";
    w4.letters = "4444";
    w5.letters = "55555";
    w6.letters = "666666";
    w7.letters = "7777777";
    w8.letters = "88888888";

    w1.next = &w2;
    w2.next = NULL;

    w3.next = &w4;
    w4.next = NULL;

    w5.next = &w6;
    w6.next = NULL;

    w7.next = &w8;
    w8.next = NULL;

    l1.words = &w1;
    l2.words = &w3;
    l3.words = &w5;
    l4.words = &w7;

    l1.next = &l2;
    l2.next = NULL;
    l2.next = &l3;
    l3.next = &l4;
    l4.next = NULL;

    r1.lines = &l1;
    r1.next = NULL;

    Region r2;
    Line l11;
    Line l12;
    Line l13;
    Line l14;

    Word w11;
    Word w12;
    Word w13;
    Word w14;
    Word w15;
    Word w16;
    Word w17;
    Word w18;

    w11.length = 1;
    w12.length = 2;
    w13.length = 3;
    w14.length = 4;
    w15.length = 5;
    w16.length = 6;
    w17.length = 7;
    w18.length = 8;

    w11.letters = "A";
    w12.letters = "BB";
    w13.letters = "CCC";
    w14.letters = "DDDD";
    w15.letters = "EEEEE";
    w16.letters = "FFFFFF";
    w17.letters = "GGGGGGG";
    w18.letters = "HHHHHHHH";

    w11.next = &w12;
    w12.next = NULL;

    w13.next = &w14;
    w14.next = NULL;

    w15.next = &w16;
    w16.next = NULL;

    w17.next = &w18;
    w18.next = NULL;

    l11.words = &w11;
    l12.words = &w13;
    l13.words = &w15;
    l14.words = &w17;

    l11.next = &l12;
    l12.next = NULL;
    l12.next = &l13;
    l13.next = &l14;
    l14.next = NULL;

    r2.lines = &l11;
    r1.next = &r2;
    r2.next = NULL;

    p.regions = &r1;
    p.next = NULL;

    r1.x = 0;
    r1.y = 0;
    r1.w = 40 * (7 + 8);
    r1.h = 40 * 4;

    r2.x = 100;
    r2.y = 0;
    r2.w = 40 * (7 + 8);
    r2.h = 40 * 4;

    l1.w = 40 * (7 + 8);
    l2.w = 40 * (7 + 8);
    l3.w = 40 * (7 + 8);
    l4.w = 40 * (7 + 8);

    l11.w = 40 * (7 + 8);
    l12.w = 40 * (7 + 8);
    l13.w = 40 * (7 + 8);
    l14.w = 40 * (7 + 8);

    l1.x = 0;
    l2.x = 0;
    l3.x = 0;
    l4.x = 0;

    l11.x = 40 * (7 + 8) + 100;
    l12.x = 40 * (7 + 8) + 100;
    l13.x = 40 * (7 + 8) + 100;
    l14.x = 40 * (7 + 8) + 100;

    l1.y = 40 * 0;
    l2.y = 40 * 1;
    l3.y = 40 * 2;
    l4.y = 40 * 3;

    l11.y = 40 * 0;
    l12.y = 40 * 1;
    l13.y = 40 * 2;
    l14.y = 40 * 3;

    l1.h = 40;
    l2.h = 40;
    l3.h = 40;
    l4.h = 40;

    l11.h = 40;
    l12.h = 40;
    l13.h = 40;
    l14.h = 40;

    r1.x = 0;
    r1.y = 0;

    r2.x = 40 * (7 + 8) + 100;
    r2.y = 0;

    r1.w = 40 * (7 + 8);
    r1.h = 40 * 4;

    r2.w = 40 * (7 + 8);
    r2.h = 40 * 4;

    p.w = 100 + 2 * (40 * (7 + 8));
    p.h = 40 * 4;

    output_save_multi_column(&p, "testpagesregions.txt");
}

int trim_demo(int argc, char *argv[]) {
    Matrix image;

    if (argc < 3) {
        printf("Missing image path.\n");
        return 1;
    }

    exit_on_error(bitmap_load(argv[2], &image));

    Matrix img = trim(&image);

    exit_on_error(bitmap_save("out.bmp", &img));

    matrix_free(&image);
    matrix_free(&img);

    return 0;
}

int scale_demo(int argc, char *argv[]) {
    Matrix image;

    if (argc < 3) {
        printf("Missing image path.\n");
        return 1;
    }

    exit_on_error(bitmap_load(argv[2], &image));

    Matrix img = scale_stretch(&image, 28, 28);

    exit_on_error(bitmap_save("out.bmp", &img));

    matrix_free(&image);
    matrix_free(&img);

    return 0;
}

int pre_process_char_demo(int argc, char *argv[]) {
    Matrix image;

    if (argc < 3) {
        printf("Missing image path.\n");
        return 1;
    }

    exit_on_error(bitmap_load(argv[2], &image));

    // image_threshold_otsu(&image);
    Matrix img = pre_process_char(&image);

    exit_on_error(bitmap_save("out.bmp", &img));

    matrix_free(&image);
    matrix_free(&img);

    return 0;
}


int all(int argc, char *argv[]) {
    Matrix image;

    if (argc < 3) {
        printf("Missing image path.\n");
        return 1;
    }

    exit_on_error(bitmap_load(argv[2], &image));

    // Sharpen & Blur
    // Contrast
    // Auto rotate
    // Segmentation
    // Trim & Scale 28x28
    // Feed to network
    // Results


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

    if (!strcmp(c, "skew_detect")) {
        return skew_detect_demo(argc, argv);
    }

    if (!strcmp(c, "auto_rotate")) {
        return auto_rotate(argc, argv);
    }

    if (!strcmp(c, "process")) {
        process_word_demo(argc, argv);
        return 0;
    }
    if (!strcmp(c, "save_pages")) {
        // save_pages_demo(argc, argv);
        save_pages_demo();
        return 0;
    }
    if (!strcmp(c, "save_pages_m")) {
        // save_pages_demo(argc, argv);
        output_save_multi_column_demo();
        return 0;
    }
    if (!strcmp(c, "trim")) {
        return trim_demo(argc, argv);
    }
    if (!strcmp(c, "scale")) {
        return scale_demo(argc, argv);
    }

    if (!strcmp(c, "pre_process")) {
        return pre_process_char_demo(argc, argv);
    }

    printf("what?\n");

    return 1;
}
