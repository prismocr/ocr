#include <stdio.h>
#include <stdlib.h>

#include "segmentation/segmentation.h"
#include "neuralnet/output.h"

#define DEFAULT_INDEX 8

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

void save_line(Line *line, size_t len_max, FILE *f) {
    size_t score = 0;
    Word *actual_word = line->words;
    while (actual_word) {
        size_t length = actual_word->length;
        for (size_t i = 0; i < length; i++) {
            save_char(actual_word->letters[i], f);
        }
        score += length;
        actual_word = actual_word->next;
        if (actual_word) {
            save_char(' ', f);
            score += 1;
        }
    }
    for (size_t i = score; i < len_max; i++) {
        save_char(' ', f);
    }
    save_char('\t', f);
}

int calculate_distance(int x1, int x2) {
    return x1 > x2 ? x1 - x2 : x2 - x1;
}

// LEFT ONLY
void get_closest_region(Page *page, RegionAround *self, size_t x, size_t y,
                        size_t size, size_t index) {
    if (!index) {
        return;
    }

    int closest = -1;
    size_t coord_x = self->me->x;
    // size_t coord_y = self->me->y;

    Region *actual_region = page->regions;
    Region *closest_region = actual_region; // ???
    while (actual_region) {
        // if the region is on the left side of the actual region
        if (actual_region->x < coord_x) {
            // if the point is in the neighbor region
            if (y > actual_region->y
                && y < actual_region->y + actual_region->h) {
                // we're sure this region is on the left side of the actual
                // region
                int distance = calculate_distance(coord_x, actual_region->x);
                if (closest == -1 || distance < closest) {
                    closest = distance;
                    closest_region = actual_region;
                }
            }
        }
        actual_region = actual_region->next;
    }
    if (self->left_distance == -1 || self->left_distance > closest) {
        self->left_distance = closest;
        self->left = closest_region;
    }
    get_closest_region(page, self, x - size / 2, y, size / 2, index - 1);
    get_closest_region(page, self, x + size / 2, y, size / 2, index - 1);
    return;
}

RegionAround check_regions_around(Page *page, Region *self) {
    RegionAround ra = {.me = self, .has_been_saved = 0};
    Region *actual_region = page->regions;
    ra.left_distance = -1;
    while (actual_region) {
        get_closest_region(page, &ra, (actual_region->x + actual_region->w) / 2,
                           (actual_region->y + actual_region->h) / 2,
                           actual_region->w / 2, DEFAULT_INDEX);
        actual_region = actual_region->next;
    }
    return ra;
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

void save_lines_multi_column(RegionAround *region_around, FILE *f) {
    size_t longest_line_of_region = 0;
    RegionAround *actual_region_around = region_around;
    while (actual_region_around) {
        Region *actual_region = actual_region_around->me;
        Line *actual_line = actual_region->lines;
        while (actual_line) { // 2n complexity
            size_t cll = compute_line_length(actual_line);
            longest_line_of_region
              = cll > longest_line_of_region ? cll : longest_line_of_region;
            actual_line = actual_line->next;
        }
        actual_line = actual_region->lines;
        while (actual_line) {
            save_line(actual_line, longest_line_of_region, f);
            actual_line = actual_line->next;
            save_char('\n', f);
        }
        actual_region_around = actual_region_around->next;
    }
}

int output_save_multi_column(Page *page, const char *path) {
    FILE *f;
    f = fopen(path, "w");
    // get longest line of each region
    Page *actual_page = page;
    while (actual_page) {
        Region *actual_region = actual_page->regions;
        while (actual_region) {
            RegionAround actual_region_around
              = check_regions_around(actual_page, actual_region);
            printf("%d\n", actual_region_around.left_distance);
            if (actual_region_around.left_distance != -1
                && actual_region_around.left != actual_region) {
                printf("Wow, this is not me\n");
                if (actual_region->next
                    && actual_region->next == actual_region_around.left) {
                    printf("Wow, the region found is the next region\n");
                }
            }
            save_lines_multi_column(&actual_region_around, f);
            actual_region = actual_region->next;
        }
        actual_page = actual_page->next;
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