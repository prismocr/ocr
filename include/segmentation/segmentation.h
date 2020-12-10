#ifndef SEGMENTATION_H
#define SEGMENTATION_H

#include "utils/matrix.h"
#include "utils/linked_list.h"
#include "utils/vector.h"

typedef struct word Word;
struct word {
    size_t x, y;
    size_t w, h;

    size_t length;
    char *letters;

    MatrixLinkedList images;
    Word *next; // next word in the line
};

typedef struct line Line;
struct line {
    size_t x, y; // coors in the region
    size_t w, h;

    Word *words;
    Line *next; // next line in the region
};

typedef struct region Region;
struct region {
    size_t x, y; // coords in the page
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

int word_new(size_t x, size_t y, size_t w, size_t h, Word **word);
void word_free(Word **word);

int line_new(size_t x, size_t y, size_t w, size_t h, Line **line);
void line_free(Line **line);

int region_new(size_t x, size_t y, size_t w, size_t h, Region **region);
void region_free(Region **region);

int page_new(size_t w, size_t h, Page **page);
void page_free(Page **page);

/**
 * Segment an image of page into its page representation.
 *
 * @param image to segment.
 * @param resulting page representation.
 */
int segment(Matrix image, Page **page);

/**
 * Segment a page into text regions using run the smooth length algorithm.
 *
 * @param page to segment.
 * @param resulting regions.
 */
int region_segment_rlsa(Matrix page, Region **regions);

/**
 * Segment a text region into lines based on morphology and histogram
 * projection.
 * More details in the following paper:
 * http://www.cvc.uab.es/icdar2009/papers/3725a651.pdf
 *
 * @param image of the text region.
 * @param resulting segmented lines.
 */
int line_segment_morph_hist(Matrix region, Line **lines);

/**
 * Segment a text region into lines using the water flow algorithm.
 * More details in the following paper:
 * https://www.researchgate.net/publication/216584186_A_New_Approach_to_Water_Flow_Algorithm_for_Text_Line_Segmentation
 *
 * @param image of the text region.
 */
int line_segment_water_flow(Matrix region);

/**
 * Segment a text line into words.
 *
 * @param image of the text line.
 * @param resulting segmented words.
 */
int word_segment(Matrix line, Word **words);

/**
 *
 */
int word_segment_prob(Matrix line, Word **words);

/**
 * Segment a word image into characters images.
 *
 * @param image of the word.
 * @param resulting characters images.
 */
int character_segment(Matrix word, MatrixLinkedList *characters);

#endif // SEGMENTATION_H
