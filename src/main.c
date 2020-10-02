#include "error.h"
#include "bitmap.h"
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

    //image_threshold(0.5f, 1.0f, &image);
    image_invert_color(&image);

    save_bmp_image("out.bmp", &image);

    return 0;
}

