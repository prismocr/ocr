#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include "utils/union_find.h"
#include "utils/error.h"

int uf_new(UnionFind *u) {
    u->capacity = UNION_FIND_DEFAULT_CAPACITY;
    u->parents = (int *) malloc(u->capacity * sizeof(int));
    if (u->parents == NULL) {
        set_last_errorf("Failed to allocate memory for union find: %s",
                        strerror(errno));
        return 1;
    }

    memset(u->parents, -1, u->capacity * sizeof(int));
    u->num_nodes = 0;

    return 0;
}

void uf_free(UnionFind *u) {
    free(u->parents);
    u->parents = NULL;
}

int uf_add_node(UnionFind *u) {
    if ((size_t) u->num_nodes == u->capacity) {
        int *new = (int *) realloc(u->parents, 2 * u->capacity * sizeof(int));
        if (new == NULL) {
            set_last_errorf("Failed to reallocate memory for union find: %s",
                            strerror(errno));
            return -1;
        }
        u->parents = new;
        memset(u->parents + u->capacity, -1, u->capacity * sizeof(int));
        u->capacity *= 2;
    }

    u->parents[u->num_nodes] = u->num_nodes;
    return u->num_nodes++;
}

int uf_find(int x, UnionFind *u) {
    if (x == -1) {
        return x;
    }

    if (x != u->parents[x]) {
        u->parents[x] = uf_find(u->parents[x], u);
    }

    return u->parents[x];
}

int uf_union(int x, int y, UnionFind *u) {
    int x_root = uf_find(x, u);
    int y_root = uf_find(y, u);

    u->parents[y_root] = x_root;

    return x_root;
}
