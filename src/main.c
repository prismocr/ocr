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

    float convo_mat[3][3] = {{0., -1., 0.}, {-1., 5., -1.}, {0., -1., 0.}};
    Matrix mat = matrix_new(3, 3);
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            mat.val[i][j] = convo_mat[i][j];
        }
    }

    sharpen(&image);

    save_bmp_image("./test.bmp", &image);
    printf("Saved image\n");
    matrix_free(&image);
    return 0;
}
