#include "bitmap.h"
#include "debug.h"
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

    try
        (bitmap_save("out.bmp", &image));

    return 0;
}
