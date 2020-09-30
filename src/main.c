#include <stdio.h>
#include "ndarray.h"
#include "image.h"
#include "error.h"

int main(int argc, char *argv[]) {
    int res;
    ndarray *im;
    res = load_bmp_image(argv[1], &im);
    if (res) {
        printf("%s\n", get_last_error());
    }
    printf("%ld %ld\n", im->dim[0], im->dim[1]);

    return 0;
}

