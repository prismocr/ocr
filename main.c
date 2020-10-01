#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "ndarray.h"
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

		//do network stuff

		//freing network
		network_free(&network);
	}
	else
    {
    	int res;
        ndarray *im;
    
        res = load_bmp_image(argv[1], &im);
        if (res) {
            printf("%s\n", get_last_error());
        }
        printf("%ld %ld\n", im->dim[0], im->dim[1]);
    }
}