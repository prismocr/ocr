#include <stddef.h>
#include "utils/matrix.h"
#include "imgproc/connected_components.h"
#include "utils/union_find.h"

void cc_labeling(Matrix *image, UnionFind *u) {
    uf_new(u);

    for (size_t i = 0; i < image->h; i++) {
        for (size_t j = 0; j < image->w; j++) {
            if (image->val[i][j] == 255.f) {
                if (i > 0 && j > 0 && image->val[i - 1][j] > 255.f
                    && image->val[i][j - 1] > 255.f
                    && image->val[i - 1][j] != image->val[i][j - 1]) {
                    image->val[i][j]
                      = 256
                        + uf_union(image->val[i - 1][j] - 256,
                                   image->val[i][j - 1] - 256, u);
                } else if (j > 0 && image->val[i][j - 1] > 255.f) {
                    image->val[i][j]
                      = 256 + uf_find(image->val[i][j - 1] - 256, u);
                } else if (i > 0 && image->val[i - 1][j] > 255.f) {
                    image->val[i][j]
                      = 256 + uf_find(image->val[i - 1][j] - 256, u);
                } else {
                    image->val[i][j] = 256 + uf_add_node(u);
                }
            }
        }
    }

    for (size_t i = 0; i < image->h; i++) {
        for (size_t j = 0; j < image->w; j++) {
            if (image->val[i][j] > 255.f) {
                image->val[i][j] = 256 + uf_find(image->val[i][j] - 256, u);
            }
        }
    }
}

void cc_bounding_box(Matrix image, float c, size_t *x, size_t *y, size_t *w,
                     size_t *h) {
    size_t top, bot, left, right;
    top = image.h;
    bot = 0;
    left = image.w;
    right = 0;
    for (size_t i = 0; i < image.h; i++) {
        for (size_t j = 0; j < image.w; j++) {
            if (image.val[i][j] == c) {
                left = j < left ? j : left;
                right = j > right ? j : right;
                top = i < top ? i : top;
                bot = i > bot ? i : bot;
            }
        }
    }

    *x = left;
    *y = top;
    *w = right - left;
    *h = bot - top;
}
