#include "image.h"
#include "ndarray.h"
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

float color_to_gray(float values[3]) {
    int val = ((float) (values[0]) * 0.07f + (float) (values[1]) * 0.71f
               + (float) (values[2]) * 0.21f);
    if (val > 255) {
        return 255;
    } else if (val < 0) {
        return 0;
    }
    return val;
}

float color_to_gray_simple(float values[3]) {
    return (values[0] + values[1] + values[2]) / 3;
}

void save_image(const char *path, const char *path_grey, ndarray *im) {
    FILE *f;
    FILE *fgrey;
    f = fopen(path, "rt");
    fgrey = fopen(path_grey, "w");

    int j, i = 0;
    int count = im->dim[0] * im->dim[1];

    for (i = 0; i < 54; i++) {
        fputc(fgetc(f), fgrey);
    }
    for (i = 0; i < count; i++) {
        for (j = 0; j < 3; j++) {
            fputc((int) im->val[i], fgrey);
        }
    }

    fclose(f);
    fclose(fgrey);
}

// TODO handle errors correctly

int load_bmp_image(const char *path, ndarray **im) {
    FILE *f;
    *im = (ndarray *) malloc(sizeof(ndarray));
    if (*im == NULL) {
        set_last_error("Failed to allocate memory for ndarray");
        return 1;
    }

    (*im)->ndim = 2;
    (*im)->dim = (size_t *) malloc(2 * sizeof(size_t));
    if ((*im)->dim == NULL) {
        set_last_error("Failed to allocate memory for dim");
        return 1;
    }

    f = fopen(path, "rb");
    if ((fgetc(f) << 8 | fgetc(f)) != 0x424d) {
        set_last_error("Invalid file format");
        return 1;
    }

    fseek(f, 18, SEEK_SET);
    (*im)->dim[0] = read_bmp_qword(f);
    (*im)->dim[1] = read_bmp_qword(f);

    fseek(f, 54, SEEK_SET);

    (*im)->val
      = (float *) malloc((*im)->dim[0] * (*im)->dim[1] * sizeof(float));

    int gray_index = 0;

    while (!feof(f)) {
        (*im)->val[gray_index]
          = color_to_gray((float[]){fgetc(f), fgetc(f), fgetc(f)});
        gray_index++;
    }
    fclose(f);

    return 0;
}
