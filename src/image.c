#include "image.h"
#include "matrix.h"
#include "error.h"
#include <stdio.h>
#include <stdlib.h>

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

unsigned int read_bmp_qword(FILE *f) {
    unsigned char b[4];
    fscanf(f, "%c%c%c%c", &b[0], &b[1], &b[2], &b[3]);
    return b[0] | b[1] << 8 | b[2] << 16 | b[3] << 24;
}

float rgb_to_gray(unsigned char color[3]) {
    return (color[0] * 0.07f + color[1] * 0.71f + color[2] * 0.21f) / 255;
}

void save_bmp_image(const char *path, Matrix *image) {
    FILE *f, *fgray;
    int i, j, c;
    f = fopen(path, "rt");
    fgray = fopen(path_gray, "w");

    for (i = 0; i < 54; i++) {
        fputc(fgetc(f), fgray);
    }
    for (i =  0; i < image->h; i++) {
        for (j = 0; j < image->w; i++) {
            for (c = 0; c < 3; c++) {
                fputc((int) image->val[i][j], fgray);
            }
        }
    }

    fclose(f);
    fclose(fgray);
}

// TODO handle errors correctly

int load_bmp_image(const char *path, Matrix *image) {
    FILE *f;

    f = fopen(path, "rb");
    if ((fgetc(f) << 8 | fgetc(f)) != 0x424d) {
        set_last_error("Invalid file format");
        return 1;
    }

    fseek(f, 18, SEEK_SET);
    *image = matrix_new(read_bmp_qword(f), read_bmp_qword(f));
    // TODO check allocation fail

    fseek(f, 54, SEEK_SET);
    int pixel = 0;
    while (!feof(f)) {
        image->val[pixel / image->w][pixel % image->w]
          = rgb_to_gray((unsigned char[]){fgetc(f), fgetc(f), fgetc(f)});
        pixel++;
    }
    fclose(f);

    return 0;
}

