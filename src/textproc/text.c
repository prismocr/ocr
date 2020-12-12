#include "textproc/dict.h"
#include "utils/utils.h"
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

void post_process_words(const char *path, Page *page) {
    Dict dict;
    dict_load(path, &dict);

    Page *actual_page = page;
    while (actual_page) {
        Region *actual_region = actual_page->regions;
        while (actual_region) {
            Line *actual_line = actual_region->lines;
            while (actual_line) {
                Word *actual_word = actual_line->words;
                while (actual_word) {
                    if (actual_word->length > 3) {
                        actual_word->candidates
                          = (char *) calloc(256, sizeof(char));
                        find_closest_word(&dict, actual_word->letters,
                                          actual_word->candidates,
                                          actual_word->length);
                        // printf("%s has a len of %zu with strlen and a len of
                        // %zu in the struct\n", actual_word->letters,
                        // strlen(actual_word->letters), actual_word->length);
                        char dest[actual_word->length];
                        strncpy(dest, actual_word->letters,
                                actual_word->length);
                        dest[actual_word->length] = '\0';
                        if (!strcmp(actual_word->candidates, dest))
                            actual_word->candidates = NULL;
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

void find_closest_word(Dict *dict, char *word, char *result, size_t len) {
    if (!len)
        len = strlen(word);

    char *w = calloc(100, sizeof(char));
    size_t min = 999;
    dict->pos = 0;

    size_t score = 0;
    size_t score_max = 10;

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
              = min3(matrix[x - 1][y] + 1, matrix[x][y - 1] + 1,
                     matrix[x - 1][y - 1] + (s1[y - 1] == s2[x - 1] ? 0 : 1));

    return (matrix[s2len][s1len]);
}
