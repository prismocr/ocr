#ifndef UNION_FIND_H
#define UNION_FIND_H

#include <stddef.h>

#define UNION_FIND_DEFAULT_CAPACITY 20

typedef struct union_find UnionFind;

/*
 * A static implementation of union find.
 * Corresponding value in parents:
 * -1: not existing
 *  i: root
 *  otherwise: parent
 */
struct union_find {
    size_t capacity;
    int *parents;

    int num_nodes;
};

int uf_new(UnionFind *u);
void uf_free(UnionFind *u);
int uf_add_node(UnionFind *u);

/*
 * Finds the equivalency class of a subset with path compression.
 */
int uf_find(int x, UnionFind *u);

int uf_union(int x, int y, UnionFind *u);

#endif // UNION_FIND_H
