#include "utils/matrix.h"
#include <math.h>
#include "imgproc/rotation.h"
#include "imgproc/image.h"
#include <stdio.h>
#include <stdlib.h>

float deg_to_rad(float angle) {
    return angle * PI / 180;
}

float rad_to_deg(float angle) {
    return angle/PI * 180;
}

void image_rotate(Matrix *image, float angle) {
    size_t i, j;

    size_t w = image->w;
    size_t h = image->h;

    float midx = w / 2;
    float midy = h / 2;

    Matrix dest;
    matrix_new(h, w, &dest);

    for (j = 0; j < h; j++) {
        for (i = 0; i < w; i++) {
            size_t x = (i - midx) * cos(angle) + (j - midy) * sin(angle) + midx;
            size_t y = (j - midx) * cos(angle) + (midx - i) * sin(angle) + midy;

            if (x < w && y < h) {
                dest.val[x][y] = image->val[i][j];
            }
        }
    }

    *image = dest;
}

float image_detect_skew(Matrix *image, float precision) {
    // Compute diagonal
    size_t diag = (size_t) sqrt(image->h*image->h* + image->w * image->w);

    // Initialize Hough Transform accumulator
    Matrix accumulator;
    matrix_new(PI*(1/precision), diag, &accumulator);

    float t_max = 0;
    float vote_max = 0;

    for(size_t y = 0; y < image->h; y++) {
        for(size_t x = 0; x < image->w; x++) {
            float c = image->val[y][x];
            if(c != 0) continue;

            // Iterate from -PI/2 to PI/2
            float t = -PI/2;
            while(t <= PI/2) {
                int d = (int) (y * cos(t) + x * sin(t));
                if(d >= 0) {
                    int teta = (int) (t*(1/precision) + (PI/2)*(1/precision));
                    accumulator.val[teta][d]++;

                    // Compute most voted value
                    if(accumulator.val[teta][d] > vote_max) {
                        vote_max = accumulator.val[teta][d];
                        t_max = t;
                    }
                }
                t += precision;
            }
        }
    }

    matrix_free(&accumulator);
    return t_max;
}

void image_auto_rotate(Matrix *image, float precision) {
    float skew = image_detect_skew(image, precision);
    image_rotate(image, skew);
}