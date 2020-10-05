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
		//creating dim array to feed to network_new()
		size_t nb_layers = 3;
		size_t sizes_static[] = {2,2,1};
		size_t *sizes = (size_t*) malloc(nb_layers*sizeof(size_t));
		memcpy(sizes, sizes_static, nb_layers*sizeof(size_t));

		//creating new network
		Network network = network_new(nb_layers,sizes);
		network.layers[1].weights.val[0][0] = 20;
		network.layers[1].weights.val[0][1] = 20;
		network.layers[1].biases[0] = -10;

		network.layers[1].weights.val[1][0] = -20;
		network.layers[1].weights.val[1][1] = -20;
		network.layers[1].biases[1] = 30;

		network.layers[2].weights.val[0][0] = 20;
		network.layers[2].weights.val[0][1] = 20;
		network.layers[2].biases[0] = -30;

		network_print(network);

		printf("\nfeeding 0 0\n");
		printf("output %f\n",network_feed_forward(&network,(float[]){0.f,0.f})[0]);

		printf("\nfeeding 0 1\n");
		printf("output %f\n",network_feed_forward(&network,(float[]){0.f,1.f})[0]);

		printf("\nfeeding 1 0\n");
		printf("output %f\n",network_feed_forward(&network,(float[]){1.f,0.f})[0]);

		printf("\nfeeding 1 1\n");
		printf("output %f\n",network_feed_forward(&network,(float[]){1.f,1.f})[0]);

		//freing network
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