#ifndef OUTPUT_H
#define OUTPUT_H

#include "segmentation/segmentation.h"

typedef struct region_around RegionAround;
struct region_around {
    int has_been_saved;
    Region *me;

    RegionAround *next;

    Region *left;
    int left_distance;
    //Region *right;
    //Region *top;
    //Region *bot;
};

void process_word(Word *w);
int output_save_default(Page *page, const char *path);
int output_save_multi_column(Page *page, const char *path);

#endif
