#ifndef OUTPUT_H
#define OUTPUT_H

#include "segmentation/segmentation.h"

void process_word(Word *w);
int output_save_default(Page *page, const char *path);
int output_save_multi_column(Page *page, const char *path);
int output_save_corrector(Page *page, const char *path);
#endif
