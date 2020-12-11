#ifndef TEXT_PROC
#define TEXT_PROC

#include <stdio.h>
#include <stdlib.h>

typedef struct dict Dict;

struct dict {
    size_t size;
    char *words;
    size_t pos;
};

void postprocessing(char *post_process);
void dict_load(const char *path, Dict *dict);
int dict_iterate(Dict *dict, char *word);
void dict_free(Dict *dict);
void dict_find_closest_word(Dict *dict, char *word, char *result);

size_t levenshtein(char *s1, char *s2);

#endif // TEXT_PROC
