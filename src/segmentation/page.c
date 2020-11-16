#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "segmentation/segmentation.h"
#include "utils/error.h"

int page_new(size_t w, size_t h, Page **page) {
    assert(*page == NULL);

    *page = (Page *) malloc(sizeof(Page));
    if (*page == NULL) {
        set_last_errorf("Failled to allocate memory for page: %s", strerror(errno));
        return 1;
    }
    (*page)->w = w;
    (*page)->h = h;
    (*page)->regions = NULL;
    (*page)->next = NULL;

    return 0;
}

void page_free(Page *page) {
    // TODO: free regions
    page->regions = NULL;

    free(page);
    page = NULL;
}

