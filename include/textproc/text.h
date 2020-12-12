#ifndef TEXT_PROC_H
#define TEXT_PROC_H

#include <stdio.h>
#include <stdlib.h>
#include "segmentation/segmentation.h"

void post_process_words(const char *path, Page *page);
/**
 * Find closest word of a word in a dictionary
 *
 * @param dict dictionary to use
 * @param word word to find the closest word of
 * @param result
 */
void find_closest_word(Dict *dict, char *word, char *result, size_t len);

/**
 * Compute the Levenshtein distance between two words
 *
 * @param s1
 * @param s2
 * @return Levenshtein distance
 */
size_t levenshtein(char *s1, char *s2);

// void postprocessing(char *post_process);

#endif // TEXT_PROC_H
