#include "bitmap.h"
#include "error.h"
#include "matrix.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BMP_HEADER_SIZE 14
#define BITMAPINFOHEADER_SIZE 40

/* Offset Size Description
 *       BMP Header
 * 0x00   2    BMP signature
 * 0x02   4    File size in bytes
 * 0x06   4    Reserved field
 * 0x0A   4    Pixel data offset
 *
 *       DIB Header (BITMAPINFOHEADER)
 * 0x0E   4    Size of the header
 * 0x12   4    Image width
 * 0x16   4    Image Height
 * 0x1A   2    Color planes
 * 0x1C   2    Bits per pixels
 * 0x1E   4    Compression method
 * 0x22   4    Raw pixel data size in bytes
 * 0x26   4    Horizontal print resolution
 * 0x2A   4    Vertical print resolution
 * 0x2E   4    Number of colors in the color palette
 * 0x32   4    Number of important colors used
 */

float bgr_to_gray(unsigned char color[3]) {
    return color[0] * 0.07f + color[1] * 0.71f + color[2] * 0.21f;
}

unsigned int read_word(FILE *f) {
    return fgetc(f) | fgetc(f) << 8;
}

unsigned int read_dword(FILE *f) {
    return fgetc(f) | fgetc(f) << 8 | fgetc(f) << 16 | fgetc(f) << 24;
}

int bitmap_load(const char *path, Matrix *image) {
    int i, j;
    unsigned int pixel_data_offset, padding, w, h;
    FILE *f;

    f = fopen(path, "rb");
    if (f == NULL) {
        set_last_errorf("Failed to open file: %s", strerror(errno));
        return 1;
    }
    if (fgetc(f) != 'B' || fgetc(f) != 'M') {
        set_last_error("Invalid image format");
        return 1;
    }

    if (fseek(f, 0xa, SEEK_SET)) {
        set_last_error("Failed to seek in file");
        return 1;
    }
    pixel_data_offset = read_dword(f);

    if (fseek(f, 0x12, SEEK_SET)) {
        set_last_error("Failed to seek in file");
        return 1;
    }
    w = read_dword(f);
    h = read_dword(f);
    if (matrix_new(h, w, image)) {
        return 1;
    }
    // TODO check allocation fail
    padding = (4 - (image->w * 3) % 4) % 4;

    if (fseek(f, 0x1c, SEEK_SET)) {
        set_last_error("Failed to seek in file");
        return 1;
    }
    if (read_word(f) != 24) {
        set_last_error("Unsupported bit depth");
        return 1;
    }

    if (fseek(f, pixel_data_offset, SEEK_SET)) {
        set_last_error("Failed to seek in file");
        return 1;
    }

    for (i = image->h - 1; i >= 0; i--) {
        for (j = 0; j < (int) image->w; j++) {
            image->val[i][j]
              = bgr_to_gray((unsigned char[]){fgetc(f), fgetc(f), fgetc(f)});
        }
        fseek(f, padding, SEEK_CUR);
        /*if (fseek(f, padding, SEEK_CUR)) {
            set_last_error("Failed to seek in file");
            return 1;
        }*/
    }
    if (fclose(f) == EOF) {
        set_last_errorf("Failed to close file: %s", strerror(errno));
        return 1;
    }

    return 0;
}

int bitmap_save(const char *path, Matrix *image) {
    unsigned char gray, padding;
    int i;
    size_t j, header_size, pixel_data_size, file_size;
    FILE *f;
    f = fopen(path, "wb");
    if (f == NULL) {
        set_last_errorf("Failed to open file: %s", strerror(errno));
        return 1;
    }

    header_size = BMP_HEADER_SIZE + BITMAPINFOHEADER_SIZE;
    padding = (4 - (image->w * 3) % 4) % 4;
    pixel_data_size = image->h * (image->w * 3 + padding);
    file_size = 54 + pixel_data_size;

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
    if (fwrite(bmp_header, 1, sizeof(bmp_header), f) != sizeof(bmp_header)) {
        set_last_errorf("Failed to write BMP header: %s", strerror(errno));
        return 1;
    }

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
    if (fwrite(dib_header, 1, sizeof(dib_header), f) != sizeof(dib_header)) {
        set_last_errorf("Failed to write DIB header: %s", strerror(errno));
        return 1;
    }

    for (i = image->h - 1; i >= 0; i--) {
        for (j = 0; j < image->w; j++) {
            gray = image->val[i][j];
            fputc(gray, f);
            fputc(gray, f);
            fputc(gray, f);
        }
        for (j = 0; j < padding; j++) {
            fputc(0, f);
        }
    }

    if (fclose(f) == EOF) {
        set_last_errorf("Failed to close file: %s", strerror(errno));
        return 1;
    }

    return 0;
}
