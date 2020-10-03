#include "bitmap.h"
#include "error.h"
#include "matrix.h"
#include <stdio.h>
#include <stdlib.h>

#define BMP_HEADER_SIZE 14
#define BITMAPINFOHEADER_SIZE 40

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

float rgb_to_gray(unsigned char color[3]) {
    return (color[0] * 0.07f + color[1] * 0.71f + color[2] * 0.21f) / 255;
}

unsigned int read_bmp_dword(FILE *f) {
    return fgetc(f) | fgetc(f) << 8 | fgetc(f) << 16 | fgetc(f) << 24;
}

unsigned int read_bmp_word(FILE *f) {
    return fgetc(f) | fgetc(f) << 8;
}

void save_bmp_image(const char *path, Matrix *image) {
    int i, j;
    FILE *f;
    f = fopen(path, "wb");

    size_t header_size = BMP_HEADER_SIZE + BITMAPINFOHEADER_SIZE;
    int padding = (4 - (image->w * 3) % 4) % 4;
    size_t pixel_data_size = image->h * (image->w * 3 + padding);
    size_t file_size = 54 + pixel_data_size;

    char bmp_header[] = {
      'B',
      'M',
      file_size & 0xff,
      file_size >> 8 & 0xff,
      file_size >> 16 & 0xff,
      file_size >> 24 & 0xff,
      0,
      0,
      0,
      0,
      header_size & 0xff,
      header_size >> 8 & 0xff,
      header_size >> 16 & 0xff,
      header_size >> 24 & 0xff,
    };
    fwrite(bmp_header, 1, sizeof(bmp_header), f);

    // BITMAPINFOHEADER
    char dib_header[] = {
      BITMAPINFOHEADER_SIZE & 0xff,
      BITMAPINFOHEADER_SIZE >> 8 & 0xff,
      BITMAPINFOHEADER_SIZE >> 16 & 0xff,
      BITMAPINFOHEADER_SIZE >> 24 & 0xff,
      image->w & 0xff,
      image->w >> 8 & 0xff,
      image->w >> 16 & 0xff,
      image->w >> 24 & 0xff,
      image->h & 0xff,
      image->h >> 8 & 0xff,
      image->h >> 16 & 0xff,
      image->h >> 24 & 0xff,
      1,
      0,
      24,
      0,
      0,
      0,
      0,
      0,
      pixel_data_size & 0xff,
      pixel_data_size >> 8 & 0xff,
      pixel_data_size >> 16 & 0xff,
      pixel_data_size >> 24 & 0xff,
      0x13,
      0x0b,
      0,
      0,
      0x13,
      0x0b,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
    };
    fwrite(dib_header, 1, sizeof(dib_header), f);

    for (i = image->h - 1; i >= 0; i--) {
        for (j = 0; j < (int) image->w; j++) {
            unsigned char gray = (unsigned char) (image->val[i][j] * 255);
            fputc(gray, f);
            fputc(gray, f);
            fputc(gray, f);
        }
        for (j = 0; j < padding; j++) {
            fputc(0, f);
        }
    }

    fclose(f);
}

// TODO handle errors correctly

int load_bmp_image(const char *path, Matrix *image) {
    int i, j;
    unsigned int pixel_data_offset, padding, w, h;
    FILE *f;

    f = fopen(path, "rb");
    if (fgetc(f) != 'B' || fgetc(f) != 'M') {
        set_last_error("Invalid image format.");
        return 1;
    }

    fseek(f, 0xa, SEEK_SET);
    pixel_data_offset = read_bmp_dword(f);

    fseek(f, 0x12, SEEK_SET);
    w = read_bmp_dword(f);
    h = read_bmp_dword(f);
    *image = matrix_new(h, w);
    // TODO check allocation fail
    padding = (4 - (image->w * 3) % 4) % 4;

    fseek(f, 0x1c, SEEK_SET);
    if (read_bmp_word(f) != 24) {
        set_last_error("Unsupported bit depth.");
        return 1;
    }

    fseek(f, pixel_data_offset, SEEK_SET);

    for (i = image->h - 1; i >= 0; i--) {
        for (j = 0; j < (int) image->w; j++) {
            image->val[i][j]
              = rgb_to_gray((unsigned char[]){fgetc(f), fgetc(f), fgetc(f)});
        }
        fseek(f, padding, SEEK_CUR);
    }
    fclose(f);

    return 0;
}
