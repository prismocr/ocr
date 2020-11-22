#include <stdio.h>
#include <stdlib.h>

#include "segmentation/segmentation.h"

// ================================================== //
// TO REMOVE
#define UNUSED(x) (void) (x)
char network_fake(Matrix *c) {
    UNUSED(*c);
    return 'a';
}
// TO REMOVE
// ================================================== //

int save_char(char c, FILE *f) {
    fputc(c, f);
    // fwrite(&c, 1, sizeof(char), f);
    return 1;
}

int output_save_default(Page *page, const char *path) {
    FILE *f;
    f = fopen(path, "w");
    Page *actual_page = page;
    while (actual_page != NULL) {
        Region *actual_region = actual_page->regions;
        while (actual_region != NULL) {
            save_char('\t', f);
            Line *actual_line = actual_region->lines;
            while (actual_line != NULL) {
                Word *actual_word = actual_line->words;
                while (actual_word != NULL) {
                    int length = actual_word->length;
                    for (int i = 0; i < length; i++) {
                        save_char(actual_word->letters[i], f);
                    }
                    actual_word = actual_word->next;
                    if (actual_word != NULL)
                        save_char(' ', f);
                }
                actual_line = actual_line->next;
                if (actual_line != NULL)
                    save_char('\n', f);
            }
            actual_region = actual_region->next;
        }
        actual_page = actual_page->next;
        if (actual_page != NULL) {
            save_char('\n', f);
            save_char('\n', f);
        }
    }
    fclose(f);
    return 0;
}

char process_char(Matrix *c) {
    // call preprocessing
    // call network
    return network_fake(c);
}

void process_word(Word *w) {
    int len = w->length;
    MatrixNode *actual_node = w->images.first;
    for (int i = 0; i < len; i++) {
        w->letters[i] = process_char(&actual_node->val);
        actual_node = actual_node->next;
    }
    return;
}