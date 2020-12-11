#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include "utils/union_find.h"
#include "utils/error.h"

int uf_new(UnionFind *u) {
    u->capacity = UNION_FIND_DEFAULT_CAPACITY;
    u->subsets = (int *) malloc(u->capacity * sizeof(int));
    if (u->subsets == NULL) {
        set_last_errorf("Failed to allocate memory for union find: %s",
                        strerror(errno));
        return 1;
    }

    memset(u->subsets, -1, u->capacity * sizeof(int));
    u->num_classes = 0;

    return 0;
}

void uf_free(UnionFind *u) {
    free(u->subsets);
    u->subsets = NULL;
}

int uf_add_subset(UnionFind *u) {
    if ((size_t) u->num_classes == u->capacity) {
        u->capacity *= 2;
        int *new = (int *) realloc(u->subsets, u->capacity * sizeof(int));
        if (new == NULL) {
            set_last_errorf("Failed to reallocate memory for union find: %s",
                            strerror(errno));
            return -1;
        }
        u->subsets = new;
    }

    u->subsets[u->num_classes] = u->num_classes;
    return u->num_classes++;
}

int uf_find(int x, UnionFind *u) {
    assert(x < u->capacity);

    if (x != u->subsets[x]) {
        u->subsets[x] = uf_find(u->subsets[x], u);
    }

    return u->subsets[x];
}

int uf_union(int x, int y, UnionFind *u) {
    int x_class = uf_find(x, u);
    int y_class = uf_find(y, u);

    u->subsets[y_class] = x_class;

    return x_class;
}
