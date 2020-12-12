#ifndef DICT_H
#define DICT_H

#include <stdio.h>
#include <stdlib.h>

typedef struct dict Dict;

struct dict {
    size_t size;
    char *words;
    size_t pos;
};

/**
 * Load dictionary into struct
 *
 * @param path path of the dictionary
 * @param dict destination struct pointer of the dictionary
 */
void dict_load(const char *path, Dict *dict);

/**
 * Iterate over dictionary words
 *
 * @param dict
 * @param word destination pointer of the next word
 * @return keep iterating
 */
int dict_iterate(Dict *dict, char *word);

/**
 * Free dictionary
 *
 * @param dict
 */
void dict_free(Dict *dict);

#endif // DICT_H
