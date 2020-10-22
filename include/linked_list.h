#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include <stdbool.h>
#include <stddef.h>
#include "matrix.h"

typedef struct matrix_node MatrixNode;

struct matrix_node {
    Matrix val;
    struct matrix_node *next;
};

typedef struct matrix_linked_list MatrixLinkedList;

struct matrix_linked_list {
    size_t length;
    MatrixNode *first;
};

void mll_new(MatrixLinkedList *list);
void mll_free(MatrixLinkedList *list);

bool mll_is_empty(MatrixLinkedList list);
Matrix *mll_get(size_t i, MatrixLinkedList list);
void mll_insert(size_t i, Matrix mat, MatrixLinkedList *list);
void mll_delete(size_t i, MatrixLinkedList *list);

void mll_print(MatrixLinkedList list);

#endif // LINKED_LIST_H
