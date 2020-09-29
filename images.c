#include <stdio.h>
#include <stdlib.h>

struct image {
    size_t ndim;
    size_t dimensions[2]; //size_t ?
    double *values;
};

int file_header[2] = {66, 77};

// --- File header ---

// signature : 2
// file size: 4
// reserved field: 4
// image offset: 4

// 14 bytes

// --- Image header ---

// size of the header: 4
// image width: 4
// image height: 4
// depth: 2
// number of bits/pixels: 2
// compression size: 4
// image size: 4 
// horizontal resolution: 4
// vertical resolution: 4
// number of palette colors: 4
// number of important colors: 4

// 40 bytes

// start at 54 bytes

// print hex value : %02x

int power16(int number) {
    if(number > 1) {
        return 16 * power16(number-1);
    }
    if(number == 0){
        return 1;
    }
    return 16;
}

double color_to_grey(double values[3]) {
    int val = ((float)(values[0])*0.07f + (float)(values[1])*0.71f + (float)(values[2])*0.21f);
    if (val > 255) {
        return 255;
    } else if (val < 0) {
        return 0;
    }
    return val;
}

double color_to_grey_simple(double values[3]) {
    return (values[0] + values[1] + values[2]) / 3;
}

void save_image(char path[], struct image im) {
    FILE *f;
    FILE *fgrey;
    f = fopen(path, "rt");
    fgrey = fopen("paint_grey.bmp", "w");

    int c, index, grey_index, i = 0;
    
    while (!feof(f) && grey_index * 8 < im.dimensions[0] * im.dimensions[1] * sizeof(double) + 24) {
        c = fgetc(f);
        if(index < 54) {
            fputc(c, fgrey);
            
        } else {
            for (int i = 0; i < 3; i++)
            {
                fputc((int)im.values[grey_index], fgrey);
            }
            grey_index++;
        }
        index++;
    }
}

struct image read_image(char path[]){

    struct image im;
    im.ndim = 2;
    FILE *f;
    f = fopen(path, "rt");

    int c, index, grey_index = 0;
    
    while (!feof(f)) {
        c = fgetc(f);
        // Make sure this is a valid BMP file
        if (index < 2) {
            if(file_header[index] != c) {
                printf("This is not a correct BMP image");
            }
        }
        index++;
        // Get and Set Dimensions
        if(index > 18 && index <= 22) {

            //im.dimensions[0] += c * power16(22 - index); // not working
            im.dimensions[0] = 200;
            // Example : 800 pixels * 800 pixels images will be in hex: 20 03, yet
            // The int "c" will give 32(10) instead of 20(16).
            // Basically, 20 * 16 + 3 * 16 * 16 = 800
            // Or we have, 32 * 16 + 3 * 16 * 16 = 1280

        } else if (index > 22 && index <= 26) {
            
            //im.dimensions[1] += c * power16(26 - index);
            im.dimensions[1] = 200;
        }
        
        // Color palette

        // Comp


        //values
        if(index >= 49) { // ???
            if (index == 49) {
                im.values = (double *) malloc(im.dimensions[0] * im.dimensions[1] * sizeof(double));
            }
            im.values[grey_index] = color_to_grey((double[]){c, fgetc(f), fgetc(f)});
            grey_index++;
        }
    }
    return im;
}

int main() {
    struct image im = read_image("paint.bmp");
    save_image("paint.bmp", im);
    free(im.values);
    return 0;
}