#include <stddef.h>
#include "utils/matrix.h"
#include "imgproc/connected_components.h"

#define MIN(a, b) (a <= b ? a : b)

void connected_components_labeling(Matrix *image, float *label) {
    for (size_t i = 0; i < image->h; i++) {
        for (size_t j = 0; j < image->w; j++) {
            if (image->val[i][j] == 255.f) {
                if (i == 0 || j == 0) {
                    if (i == 0 && j != 0 && image->val[i][j - 1] >= 1.) {
                        image->val[i][j] = image->val[i][j - 1];
                    } else if (i != 0 && j == 0 && image->val[i - 1][j] >= 1.) {
                        image->val[i][j] = image->val[i - 1][j];
                    } else {
                        image->val[i][j] = (*label)++;
                    }
                } else if (image->val[i - 1][j] < 1.f
                           || image->val[i][j - 1] < 1.f) {
                    if (image->val[i - 1][j] > 1.f) {
                        image->val[i][j] = image->val[i - 1][j];
                    } else if (image->val[i][j - 1] > 1.) {
                        image->val[i][j] = image->val[i][j - 1];
                    } else {
                        image->val[i][j] = (*label)++;
                    }
                } else {
                    image->val[i][j]
                      = MIN(image->val[i - 1][j], image->val[i][j - 1]);
                }
            }
        }
    }

    for (size_t i = image->h; i > 0;) {
        i -= 1;
        for (size_t j = image->w; j > 0;) {
            j -= 1;
            if (image->val[i][j] > 1.f) {
                if (i == image->h - 1 || j == image->w - 1) {
                    if (i == image->h - 1 && j != image->w - 1
                        && image->val[i][j + 1] > 1.f) {
                        image->val[i][j] = image->val[i][j + 1];
                    } else if (i != image->h - 1 && j == image->w - 1
                               && image->val[i + 1][j] > 1.f) {
                        image->val[i][j] = image->val[i + 1][j];
                    }
                } else if (image->val[i + 1][j] < 1.f
                           || image->val[i][j + 1] < 1.f) {
                    if (image->val[i + 1][j] > 1.f) {
                        image->val[i][j] = image->val[i + 1][j];
                    } else if (image->val[i][j + 1] > 1.f) {
                        image->val[i][j] = image->val[i][j + 1];
                    }
                } else {
                    image->val[i][j]
                      = MIN(image->val[i + 1][j], image->val[i][j + 1]);
                }
            }
        }
    }
}
