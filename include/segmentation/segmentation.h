#ifndef SEGMENTATION_H
#define SEGMENTATION_H

#include "utils/matrix.h"
#include "utils/linked_list.h"
#include "utils/vector.h"

typedef struct word Word;
struct word {
    size_t length;
    char *letters;

    Matrix *images;
};

typedef struct line Line;
struct line {
    size_t x, y;
    size_t w, h;

    Word *words;
};

typedef struct region Region;
struct region {
    size_t x, y;
    size_t w, h;

    Line *lines;
    Region *next; // next region in the page
};

typedef struct page Page;
struct page {
    size_t w, h;

    Region *regions;
    Page *next;
};

int region_new(size_t x, size_t y, size_t w, size_t h, Region **region);
void region_free(Region *region);

int page_new(size_t w, size_t h, Page **page);
void page_free(Page *page);

/**
 * Segment an image.
 */
int segment(Matrix image, Page *page);

/**
 *
 */
int segment_regions_rlsa(Matrix image, Region **regions);

// Should return list of words
MatrixLinkedList segment_words(Matrix image, Vector hist);

void segment_morph_hist(Matrix image);
void feature_extract_morph_based(Matrix *image);

void segment_water_flow(Matrix image);
void morphological_preproc(Matrix *image);

#endif // SEGMENTATION_H
