#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "segmentation/segmentation.h"
#include "imgproc/image.h"
#include "utils/matrix.h"
#include "imgproc/morphology.h"
#include "utils/linked_list.h"
#include "utils/error.h"

// TODO remove these headers
#include "utils/bitmap.h"
#include <stdio.h>

int segment(Matrix image, Page **page) {
    if (page_new(image.w, image.h, page)) {
        return 1;
    }

    if (region_segment_rlsa(image, &(*page)->regions)) {
        return 1;
    }

    char buff[200];
    size_t i = 0;
    Region *prev_region = NULL;
    for (Region *region = (*page)->regions; region != NULL;) {
        Matrix region_image
          = image_crop(region->x, region->y, region->w, region->h, image);
        line_segment_morph_hist(region_image, &region->lines);

        size_t j = 0;
        Line *prev_line = NULL;
        for (Line *line = region->lines; line != NULL;) {
            Matrix line_image
              = image_crop(region->x + line->x, region->y + line->y, line->w,
                           line->h, image);
            word_segment(line_image, &line->words);

            size_t k = 0;
            for (Word *word = line->words; word != NULL; word = word->next) {
                Matrix word_image = image_crop(region->x + line->x + word->x,
                                               region->y + line->y + word->y,
                                               word->w, word->h, image);
                sprintf(buff, "seg/word-%zu-%zu-%zu.bmp", i, j, k);
                bitmap_save(buff, &word_image);

                character_segment(word_image, &word->images);
                word->length = word->images.length;
                word->letters
                  = (char *) calloc((1 + word->length), sizeof(char));
                if (word->letters == NULL) {
                    set_last_errorf(
                      "Failled to allocate memory for letters: %s",
                      strerror(errno));
                    return 1;
                }

                for (size_t l = 0; l < word->images.length; l++) {
                    sprintf(buff, "seg/char-%zu-%zu-%zu-%zu.bmp", i, j, k, l);
                    bitmap_save(buff, mll_get(l, word->images));
                }

                k++;
                matrix_free(&word_image);
            }

            if (line->words == NULL) {
                Line *next = line->next;
                line_free(&line);
                line = next;
                if (prev_line == NULL) {
                    region->lines = next;
                } else {
                    prev_line->next = next;
                }
            } else {
                sprintf(buff, "seg/line-%zu-%zu.bmp", i, j);
                bitmap_save(buff, &line_image);

                prev_line = line;
                line = line->next;
            }

            j++;
            matrix_free(&line_image);
        }

        if (region->lines == NULL) {
            Region *next = region->next;
            region_free(&region);
            region = next;
            if (prev_region == NULL) {
                (*page)->regions = next;
            } else {
                prev_region->next = next;
            }
        } else {
            sprintf(buff, "seg/region-%zu.bmp", i);
            bitmap_save(buff, &region_image);

            prev_region = region;
            region = region->next;
        }

        i++;
        matrix_free(&region_image);
    }

    return 0;
}
