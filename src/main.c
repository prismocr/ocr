#include "bitmap.h"
#include "error.h"
#include "image.h"
#include "matrix.h"
#include "convolution.h"
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

    // image_contrast(&image, 180.f);

    

    sharpen(&image);

    save_bmp_image("./test.bmp", &image);
    printf("Saved image\n");
    matrix_free(&image);
    return 0;
}
