#include "textproc/dict.h"

void dict_load(const char *path, Dict *dict) {
    FILE *f = fopen(path, "rb");
    fseek(f, 0, SEEK_END);
    dict->size = (size_t) ftell(f);
    dict->pos = 0;
    fseek(f, 0, SEEK_SET); /* same as rewind(f); */

    dict->words = malloc(dict->size + 1);

    size_t e = fread(dict->words, 1, dict->size, f);
    if (e)
        printf("%ld\n", e);
    fclose(f);

    dict->words[dict->size] = 0;
}

int dict_iterate(Dict *dict, char *word) {
    size_t k = dict->pos;
    size_t i = 0;
    while (dict->words[k] != '\n' && dict->words[k] != 0) {
        *(word + i) = dict->words[k];
        i++;
        k++;
    }
    *(word + i) = 0;
    dict->pos = k + 1;

    return dict->pos <= dict->size - 1;
}

void dict_free(Dict *dict) {
    free(dict->words);
}