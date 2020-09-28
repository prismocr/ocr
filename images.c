#include <stdio.h>

struct image {
    FILE *file;
    int dimensions[2];
    int *value;
};

FILE *fr;

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

// TODO : remove height/width array


int file_height[4];
int file_width[4];

int height = 0;
int width = 0;

int power16(int number) {
    if(number > 1) {
        return 16 * power16(number-1);
    }
    if(number == 0){
        return 1;
    }
    return 16;
}

int main(){

    struct image image;
    image.file = fopen("lena.bmp", "rt");

    int c;
    int val;
    int index = 0;

    while (!feof(image.file)) {
        c = fgetc(image.file);
        if (index < 2) {
            if(file_header[index] != c) {
                printf("This is not a correct BMP image");
            }
        }
        index++;

        if(index > 18 && index <= 22) {
            file_width[(index - 3) % 4] = c;
        } else if (index > 22 && index <= 26) {
            file_height[(index - 3) % 4] = c;
        }
    }

    for (index = 0 ; index < 4 ; index++) {
        width += file_width[index] * power16(3-index);
    }

    for (index = 0 ; index < 4 ; index++) {
        height += file_width[index] * power16(3-index);
    }
    
    printf("width : %d, height : %d\n", width, height);
    image.dimensions[0] = width;
    image.dimensions[1] = height;
}
