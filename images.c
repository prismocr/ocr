#include "images.h"
#include <stdio.h>
#include <stdlib.h>

int file_header[2] = {66, 77};

// --- File header ---

// signature : 2
// file size: 4
// reserved field: 4
// image offset: 4

// 14 bytes

// --- Image header ---

// size of the header: 4
// image width: 4
// image height: 4
// depth: 2
// number of bits/pixels: 2
// compression size: 4
// image size: 4
// horizontal resolution: 4
// vertical resolution: 4
// number of palette colors: 4
// number of important colors: 4

// 40 bytes

// start at 54 bytes

// print hex value : %02x

unsigned int read_bmp_qword(FILE *f) {
    unsigned char b[4];
    fscanf(f, "%c%c%c%c", &b[0], &b[1], &b[2], &b[3]);
    return b[0] | b[1] << 8 | b[2] << 16 | b[3] << 24;
}

double color_to_grey(double values[3]) {
    int val = ((float) (values[0]) * 0.07f + (float) (values[1]) * 0.71f
               + (float) (values[2]) * 0.21f);
    if (val > 255) {
        return 255;
    } else if (val < 0) {
        return 0;
    }
    return val;
}

double color_to_grey_simple(double values[3]) {
    return (values[0] + values[1] + values[2]) / 3;
}

void save_image(char path[], char path_grey[], struct image im) {
    FILE *f;
    FILE *fgrey;
    f = fopen(path, "rt");
    fgrey = fopen(path_grey, "w");

    int j, i = 0;
    int count = im.dimensions[0] * im.dimensions[1];

    for (int i = 0; i < 54; i++) {
        fputc(fgetc(f), fgrey);
    }
    for (int i = 0; i < count; i++) {
        for (int j = 0; j < 3; j++) {
            fputc((int) im.values[i], fgrey);
        }
    }

    fclose(f);
    fclose(fgrey);
}

struct image load_image_grey(char path[]) {
    struct image im;
    im.ndim = 2;
    FILE *f;

    f = fopen(path, "rt");

    if (fgetc(f) != 0x42 || fgetc(f) != 0x4D) {
        printf("This is no a correct BMP image");
    }

    fseek(f, 18, SEEK_SET);

    im.dimensions[0] = read_bmp_qword(f);
    im.dimensions[1] = read_bmp_qword(f);

    fseek(f, 54, SEEK_SET);

    im.values
      = (double *) malloc(im.dimensions[0] * im.dimensions[1] * sizeof(double));

    int grey_index = 0;

    while (!feof(f)) {
        im.values[grey_index]
          = color_to_grey((double[]){fgetc(f), fgetc(f), fgetc(f)});
        grey_index++;
    }
    fclose(f);
    return im;
}