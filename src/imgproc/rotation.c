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
    return angle / PI * 180;
}

void image_rotate(Matrix *image, float angle) {
    int w = image->w;
    int h = image->h;
    Matrix dest;
    matrix_new(h, w, &dest);

    // Initialize rotated image matrice
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            dest.val[y][x] = -1;
        }
    }

    // Initialize rotation matrice
    Matrix rot;
    matrix_new(2, 2, &rot);
    rot.val[0][0] = cos(angle);
    rot.val[0][1] = -sin(angle);
    rot.val[1][0] = sin(angle);
    rot.val[1][1] = cos(angle);

    float midx = w / 2;
    float midy = h / 2;

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            float c = image->val[y][x];

            Matrix col;
            matrix_new(2, 1, &col);
            col.val[0][0] = x - midx;
            col.val[1][0] = y - midy;

            // Compute matrice rotation
            Matrix r;
            matrix_dot(rot, col, &r);

            // Compute new position
            float posX = r.val[0][0] + midx + 1;
            float posY = r.val[1][0] + midy + 1;

            // Rotate
            if (posX < w && posY < h && posX >= 0 && posY >= 0) {
                dest.val[(int) posY][(int) posX] = c;
            }

            matrix_free(&col);
            matrix_free(&r);
        }
    }

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            if (dest.val[y][x] == -1) {
                dest.val[y][x] = anti_aliasing_point(&dest, y, x);
            }
        }
    }

    matrix_free(&rot);
    matrix_free(image);
    *image = dest;
}

float image_detect_skew(Matrix *image, float precision) {
    // Compute diagonal
    size_t diag = (size_t) sqrt(image->h * image->h + image->w * image->w);

    // Initialize Hough Transform accumulator
    Matrix accumulator;
    matrix_new(PI * (1 / precision), diag, &accumulator);

    float t_max = 0;
    float vote_max = 0;

    for (size_t y = 0; y < image->h; y++) {
        for (size_t x = 0; x < image->w; x++) {
            float c = image->val[y][x];
            if (c != 0)
                continue;

            // Iterate from -PI/2 to PI/2
            float t = -PI / 2;
            while (t <= PI / 2) {
                int d = (int) (y * cos(t) + x * sin(t));
                if (d >= 0) {
                    int teta = (int) (t * (1 / precision)
                                      + (PI / 2) * (1 / precision));
                    accumulator.val[teta][d]++;

                    // Compute most voted value
                    if (accumulator.val[teta][d] > vote_max) {
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

    if (fabs(rad_to_deg(skew)) <= 1.f)
        return;

    image_rotate(image, skew);
}