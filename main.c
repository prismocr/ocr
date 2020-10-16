#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "bitmap.h"
#include "image.h"
#include "error.h"
#include "matrix.h"
#include "network.h"
#include "layer.h"

int main(int argc, char *argv[]) {
	if(argc < 2){
		srand(time(NULL));
		//create dim array to feed to network_new()
		size_t nb_layers = 3;
		size_t sizes_static[] = {2,2,1};

		//create new network
		Network network = network_new(nb_layers,sizes_static);

        // --- Create dataset---
    	Dataset dataset;
    	dataset_new(&dataset,4);

        Vector vect00, vect01, vect10, vect11, vect0, vect1;
        vector_new(2,&vect00);
        vector_copy(2,(float[]){0,0},vect00.val);
        vector_new(2,&vect01);
        vector_copy(2,(float[]){0,1},vect01.val);
        vector_new(2,&vect10);
        vector_copy(2,(float[]){1,0},vect10.val);
        vector_new(2,&vect11);
        vector_copy(2,(float[]){1,1},vect11.val);
        vector_new(1,&vect0);
        vector_copy(1,(float[]){0},vect0.val);
        vector_new(1,&vect1);
        vector_copy(1,(float[]){1},vect1.val);

    	Data data00,data01,data10,data11;
    	data00.input = vect00;
    	data00.target = vect0;
    	data01.input = vect01;
    	data01.target = vect1;
    	data10.input = vect10;
    	data10.target = vect1;
    	data11.input = vect11;
    	data11.target = vect0;

    	dataset.datas[0] = data00;
    	dataset.datas[1] = data01;
    	dataset.datas[2] = data10;
    	dataset.datas[3] = data11;

    	network_sgd(&network, &dataset, 1, 4, 2.5f);

    	dataset_free(&dataset);
		network_free(&network);
	}
	else
    {
    	Matrix image;

	    if (argc < 2) {
	        printf("Missing image path.\n");
	        return 1;
	    }

	    try
	        (bitmap_load(argv[1], &image));

	    try
	        (bitmap_save("out.bmp", &image));
    }
}