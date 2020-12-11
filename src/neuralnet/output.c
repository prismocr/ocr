#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "segmentation/segmentation.h"
#include "neuralnet/output.h"

#define SIMULATE_CHAR_SIZE 40

// ================================================== //
// TO REMOVE
#define UNUSED(x) (void) (x)
char network_fake(Matrix *c) {
    UNUSED(*c);
    return 'a';
}
// TO REMOVE
// ================================================== //

void PRINT_CHAR_ARRAY(char string[]) {
    char char_actual = string[0];
    size_t index = 1;
    while (char_actual) {
        printf("\033[1;33m%c  ", char_actual);
        char_actual = string[index];
        index++;
    }
    printf("\n\033[0m");
    char_actual = string[0];
    index = 1;
    while (char_actual) {
        printf("\033[1;33m%d ", char_actual);
        char_actual = string[index];
        index++;
    }
    printf("\n\033[0m");
}

int save_char(char c, FILE *f) {
    fputc(c, f);
    // fwrite(&c, 1, sizeof(char), f);
    return 1;
}

void save_char_array(char string_to_save[], FILE *f) {
    char char_actual = string_to_save[0];
    size_t index = 1;
    while (char_actual) {
        save_char(char_actual, f);
        char_actual = string_to_save[index];
        index++;
    }
}

int output_save_default(Page *page, const char *path) {
    FILE *f;
    f = fopen(path, "w");
    Page *actual_page = page;
    while (actual_page) {
        Region *actual_region = actual_page->regions;
        while (actual_region) {
            save_char('\t', f);
            Line *actual_line = actual_region->lines;
            while (actual_line) {
                Word *actual_word = actual_line->words;
                while (actual_word) {
                    size_t length = actual_word->length;
                    for (size_t i = 0; i < length; i++) {
                        save_char(actual_word->letters[i], f);
                    }
                    actual_word = actual_word->next;
                    if (actual_word)
                        save_char(' ', f);
                }
                actual_line = actual_line->next;
                if (actual_line)
                    save_char('\n', f);
            }
            actual_region = actual_region->next;
            save_char('\n', f);
        }
        actual_page = actual_page->next;
        if (actual_page) {
            save_char('\n', f);
            save_char('\n', f);
        }
    }
    fclose(f);
    return 0;
}

int output_save_corrector(Page *page, const char *path) {
    FILE *f;
    f = fopen(path, "w");
    Page *actual_page = page;
    while (actual_page) {
        Region *actual_region = actual_page->regions;
        while (actual_region) {
            save_char('\t', f);
            Line *actual_line = actual_region->lines;
            while (actual_line) {
                Word *actual_word = actual_line->words;
                while (actual_word) {
                    if (!actual_word->candidates) {
                        size_t length = actual_word->length;
                        for (size_t i = 0; i < length; i++) {
                            save_char(actual_word->letters[i], f);
                        }
                    } else {
                        save_char('[', f);
                        size_t length = actual_word->length;
                        for (size_t i = 0; i < length; i++) {
                            save_char(actual_word->letters[i], f);
                        }
                        save_char('=', f);
                        length = strlen(actual_word->candidates);
                        // printf("Len = %zu, %c\n", length, );
                        for (size_t i = 0; i < length; i++) {
                            save_char(actual_word->candidates[i], f);
                        }
                        save_char(']', f);
                    }

                    actual_word = actual_word->next;
                    if (actual_word)
                        save_char(' ', f);
                }
                actual_line = actual_line->next;
                if (actual_line)
                    save_char('\n', f);
            }
            actual_region = actual_region->next;
            save_char('\n', f);
        }
        actual_page = actual_page->next;
        if (actual_page) {
            save_char('\n', f);
            save_char('\n', f);
        }
    }
    fclose(f);
    return 0;
}

size_t verify_coordinates(size_t x, size_t y, size_t x2, size_t y2, size_t h,
                          size_t w) {
    return x >= x2 && x <= (x2 + w) && y >= y2 && y <= (y2 + h) ? 1 : 0;
}

int calculate_distance(int x1, int x2) {
    return x1 > x2 ? x1 - x2 : x2 - x1;
}

size_t compute_line_length(Line *line) {
    size_t score = 0;
    Word *actual_word = line->words;
    while (actual_word) {
        score += actual_word->length;
        actual_word = actual_word->next;
        if (actual_word) {
            score += 1;
        }
    }
    return score;
}

size_t calculte_mean(Page *page) {
    Page *actual_page = page;
    size_t total = 0;
    size_t index = 0;
    if (!page) {
        printf("uh oh page = null");
        return 1;
    }
    while (page) {
        Region *actual_region = actual_page->regions;
        while (actual_region) {
            Line *actual_line = actual_region->lines;
            while (actual_line) {
                index++;
                total += actual_line->h;
                actual_line = actual_line->next;
            }
            actual_region = actual_region->next;
        }
        page = page->next;
    }
    return total / index;
}

int output_save_multi_column(Page *page, const char *path) {
    FILE *f;
    f = fopen(path, "w");

    // STEP 1 : We get the average of :
    //  - size of char (40)

    // TODO CALCULATE IT
    size_t average_size_char = calculte_mean(page);

    printf("Average_size_char = %zu\n", average_size_char);

    // STEP 2 : Loop through x and y
    Page *actual_page = page;
    size_t x, y;
    while (actual_page) {
        // Loop through x and y
        // We start with an offset to make sure to encounter each lines;
        y = average_size_char / 2;
        // printf("\nPAGES SIZES %zu %zu\n", actual_page->h, actual_page->w);
        while (y < actual_page->h) {
            // printf("%zu %zu %zu\n", actual_page->w, average_size_char,
            //    actual_page->w / average_size_char);

            // if segfault, probably here:
            size_t size_max_char_pages
              = (actual_page->w / average_size_char) * 10 + 1;
            char line_as_string[size_max_char_pages];
            size_t index = 0;

            for (size_t i = 0; i < size_max_char_pages; i++) {
                line_as_string[i] = 0;
            }

            line_as_string[size_max_char_pages - 1] = 0;
            x = 0;

            // printf("\nNEW ITERATION\n\n");
            while (x < actual_page->w) {
                // STEP 3 : is x and y in a region ?
                Region *actual_region = actual_page->regions;
                while (actual_region) {
                    size_t test_coordinates_region = verify_coordinates(
                      x, y, actual_region->x, actual_region->y,
                      actual_region->h, actual_region->w);
                    if (test_coordinates_region) {
                        // printf("\033[1;31mYOU'RE INSIDE A REGION x=%zu y=%zu
                        // w=%zu h=%zu\n\033[0m", actual_region->x,
                        // actual_region->y, actual_region->w,
                        // actual_region->h); STEP 3.5 : Search longest line
                        Line *actual_line = actual_region->lines;
                        size_t longest_line_of_region = 0;
                        while (actual_line) {
                            size_t cll = compute_line_length(actual_line);
                            longest_line_of_region
                              = cll > longest_line_of_region
                                  ? cll
                                  : longest_line_of_region;
                            actual_line = actual_line->next;
                        }
                        actual_line = actual_region->lines;
                        // STEP 4 : Found which line
                        // printf("Actual line x = %zu, y = %zu\n\n", x, y);
                        while (actual_line) {
                            // printf("line x = %zu y = %zu h = %zu w = %zu\n",
                            // actual_line->x, actual_line->y, actual_line->h,
                            // actual_line->w); printf("line offset x = %zu y =
                            // %zu h = %zu w = %zu\n", actual_line->x +
                            // actual_region->x, actual_line->y +
                            // actual_region->y, actual_line->h,
                            // actual_line->w);
                            size_t test_coordinates_region = verify_coordinates(
                              x, y, actual_line->x + actual_region->x,
                              actual_line->y + actual_region->y, actual_line->h,
                              actual_line->w);
                            if (test_coordinates_region) {
                                // printf("\033[1;32mYOU'RE INSIDE A
                                // LINE\n\033[0m");
                                // STEP 5 : add this to the current string to
                                // save
                                Word *actual_word = actual_line->words;
                                size_t len_of_line = 0;
                                while (actual_word) {
                                    // printf("\033[1;34mYOU'RE INSIDE A
                                    // WORD\n\033[0m");
                                    for (size_t word_index = 0;
                                         word_index < actual_word->length;
                                         word_index++, index++) {
                                        line_as_string[index]
                                          = actual_word->letters[word_index];
                                        len_of_line++;
                                    }
                                    line_as_string[index] = ' ';
                                    index++;
                                    actual_word = actual_word->next;
                                }
                                for (; len_of_line < longest_line_of_region;
                                     len_of_line++) {
                                    line_as_string[index] = ' ';
                                    index++;
                                }
                                // STEP 6 : increment x by the line size + 1char
                                x += actual_line->w + average_size_char;
                                // printf("DOUBLE BREAKING\n");
                                goto DOUBLE_BREAK;
                                // Need Double Break
                            } else {
                                actual_line = actual_line->next;
                            }
                            // printf("Not the right line man...\n");
                        }
                    }
                    actual_region = actual_region->next;
                }
            DOUBLE_BREAK:
                x += average_size_char;
                line_as_string[index] = ' ';
                index++;
            }
            // STEP 7 : save the line
            // printf("SAVING LINE:\n");
            // PRINT_CHAR_ARRAY(line_as_string);
            save_char_array(line_as_string, f);
            save_char('\n', f);
            y += average_size_char;
        }
        actual_page = actual_page->next;
        if (actual_page)
            save_char('\n', f);
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