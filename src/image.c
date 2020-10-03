#include "matrix.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

void image_threshold(float thresh, float maxval, Matrix *image) {
    size_t i, j;

    for (i = 0; i < image->h; i++) {
        for (j = 0; j < image->w; j++) {
            image->val[i][j] = image->val[i][j] < thresh ? 0 : maxval;
        }
    }
}

void image_invert_color(Matrix *image) {
    size_t i, j;

    for (i = 0; i < image->h; i++) {
        for (j = 0; j < image->w; j++) {
            image->val[i][j] = 1 - image->val[i][j];
        }
    }
}

float Restrict(float value) {
    if(value > 255) return 255;
    if(value < 0) return 0;

    return value;
}

void image_contrast(Matrix *image, float delta) {
    size_t i, j;
    double factor = (259.f * (delta + 255.f)) / (255.f * (259.f - delta));

    for (i = 0; i < image->h; i++) {
        for (j = 0; j < image->w; j++) {
            float color = factor * (image->val[i][j] * 255.f - 128) + 128;
            image->val[i][j] = Restrict(color) / 255.f;
        }
    }
} 

void image_rotate(Matrix *image, float angle) {
    size_t i, j;
    
    size_t w = image->w;
    size_t h = image->h;

    float midx = w/2;
    float midy = h/2;

    Matrix dest = matrix_new(h, w);

    for(j = 0; j < h; j++) {
        for(i = 0; i < w; i++) {
            size_t x = (i - midx) * cos(angle) + (j - midy) * sin(angle) + midx;
            size_t y = (j - midx) * cos(angle) + (midx - i) * sin(angle) + midy;
            
            if(x < w && y < h) {
                dest.val[x][y] = image->val[i][j];
            }
        }
    }

    *image = dest;
}