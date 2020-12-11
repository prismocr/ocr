#include "textproc/text.h"
#include "segmentation/segmentation.h"
#include <ctype.h>
#include <string.h>

// TODO : Rewrite
void postprocessing(char *post_process) {
    FILE *f_read;
    FILE *f_save;
    f_read = fopen(post_process, "r");
    f_save = fopen("post_processed.txt", "w");
    fseek(f_read, 0, SEEK_END);
    long size = ftell(f_read);
    fseek(f_read, 0, SEEK_SET);
    printf("size = %ld\n", size);
    char str[size];

    for (long i = 0; i < size; i++)
        str[i] = fgetc(f_read);

    char s = ' ';
    size_t dot_distance = 0;

    for (long i = 0; i < size; i++) {
        s = str[i];
        if (!dot_distance && s != ' ' && s != '\n' && s != '\t') {
            s = toupper(s);
            dot_distance = 1;
        }
        if (s == '.') {
            dot_distance = 0;
        }
        fwrite(&s, sizeof(char), 1, f_save);
    }
    fclose(f_read);
    fclose(f_save);
}

void post_process_words(Page *page) {
    Dict dict;
    dict_load("./assets/words_en.txt", &dict);

    Page *actual_page = page;
    while (actual_page) {
        Region *actual_region = actual_page->regions;
        while (actual_region) {
            Line *actual_line = actual_region->lines;
            while (actual_line) {
                Word *actual_word = actual_line->words;
                while (actual_word) {
                    if (actual_word->length > 3) {
                        actual_word->candidates = calloc(256, sizeof(char));
                        dict_find_closest_word(&dict, actual_word->letters,
                                               actual_word->candidates);
                        // printf("closest of %s is %s and len is %zu\n",
                        // actual_word->letters, actual_word->candidates,
                        // actual_word->length);
                    } else {
                        actual_word->candidates = NULL;
                    }
                    actual_word = actual_word->next;
                }
                actual_line = actual_line->next;
            }
            actual_region = actual_region->next;
        }
        actual_page = actual_page->next;
    }
    dict_free(&dict);
}

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
void dict_find_closest_word(Dict *dict, char *word, char *result) {
    if (strlen(word) == 3) {
        strcpy(result, word);
        return;
    }

    char *w = calloc(100, sizeof(char));
    size_t min = 999;
    dict->pos = 0;

    size_t score = 0;
    size_t score_max = 10;

    size_t len = strlen(word) - 1; // STRTOK !!

    while (dict_iterate(dict, w)) {
        if (!strcmp(w, word)) {
            strcpy(result, word);
            break;
        }

        if (strlen(w) != len)
            continue;

        size_t distance = levenshtein(w, word);
        if (distance < min) {
            min = distance;
            strcpy(result, w);
            score = 0;
        } else if (score < score_max && distance == min
                   && strlen(result) + strlen(w) < 256) {
            strcat(result, "/");
            strcat(result, w);
            score++;
        }

        if (distance == 0)
            break;
    }

    free(w);
    dict->pos = 0;
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

int min(int a, int b, int c) {
    return a < b ? (a < c ? a : c) : (b < c ? b : c);
}

size_t levenshtein(char *s1, char *s2) {
    size_t x, y, s1len, s2len;
    s1len = strlen(s1);
    s2len = strlen(s2);
    size_t matrix[s2len + 1][s1len + 1];
    matrix[0][0] = 0;

    for (x = 1; x <= s2len; x++)
        matrix[x][0] = matrix[x - 1][0] + 1;
    for (y = 1; y <= s1len; y++)
        matrix[0][y] = matrix[0][y - 1] + 1;
    for (x = 1; x <= s2len; x++)
        for (y = 1; y <= s1len; y++)
            matrix[x][y]
              = min(matrix[x - 1][y] + 1, matrix[x][y - 1] + 1,
                    matrix[x - 1][y - 1] + (s1[y - 1] == s2[x - 1] ? 0 : 1));

    return (matrix[s2len][s1len]);
}