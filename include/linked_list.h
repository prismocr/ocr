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

/**
 * Create a new matrix linke list.
 *
 * @param a pointer to output the new matrix linked list.
 */
void mll_new(MatrixLinkedList *list);

/**
 * Free a matrix linked list.
 *
 * @param the matrix linked list to free.
 */
void mll_free(MatrixLinkedList *list);

/**
 * Check if a matrix linked list is not empty.
 *
 * @param a matrix linked list.
 * @return is the matrix linked empty.
 */
bool mll_is_empty(MatrixLinkedList list);

/**
 * Get the matrix at index i of a matrix linked list.
 *
 * @param index of the matrix to get.
 * @param a matrix linked list.
 * @return a pointer to the element.
 */
Matrix *mll_get(size_t i, MatrixLinkedList list);

/**
 * Insert a matrix into a matrix linked list.
 *
 * @param index to insert matrix at.
 * @param matrix to insert.
 * @param a matrix linked list.
 */
void mll_insert(size_t i, Matrix mat, MatrixLinkedList *list);

/**
 * Delete an element from a matrix linked list.
 *
 * @param index of the element to delete.
 * @param a matrix linked list.
 */
void mll_delete(size_t i, MatrixLinkedList *list);

/**
 * Print values of a matrix linked list.
 *
 * @param a matrix linked list.
 */
void mll_print(MatrixLinkedList list);

#endif // LINKED_LIST_H
