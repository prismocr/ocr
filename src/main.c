#include "bitmap.h"
#include "error.h"
#include "image.h"
#include "matrix.h"
#include "segmentation.h"
#include <stdio.h>

int main(int argc, char *argv[]) {
    Matrix image;

    if (argc < 2) {
        printf("Missing image path.\n");
        return 1;
    }

    try
        (bitmap_load(argv[1], &image));

    feature_extraction(&image);

    try
        (bitmap_save("out.bmp", &image));

    matrix_free(&image);

    return 0;
}
