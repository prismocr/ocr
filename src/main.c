#include "demo.h"
#include "neuralnet/data.h"

int main(int argc, char *argv[]) {
    //return demo(argc, argv);
    argc = argc;
    argv = argv;
    Dataset dataset;
	generate_dataset("dataset/",&dataset);
}
