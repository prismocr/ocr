#include "bitmap.h"
#include "error.h"
#include "image.h"
#include "matrix.h"
#include <stdio.h>

int main(int argc, char *argv[]) {
    int res;
    Matrix image;
    if (argc < 2) {
        printf("Missing image path.\n");
        return 1;
    }

    res = load_bmp_image(argv[1], &image);
    if (res) {
        printf("%s\n", get_last_error());
        return 1;
    }

    return 0;
}
