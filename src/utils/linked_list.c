#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include "utils/matrix.h"
#include "utils/linked_list.h"

void mll_new(MatrixLinkedList *list) {
    *list = (MatrixLinkedList){0, NULL};
}

void mll_free(MatrixLinkedList *list) {
    MatrixNode *current = list->first;
    MatrixNode *temp = NULL;

    while (current != NULL) {
        temp = current->next;
        matrix_free(&current->val);
        free(current);
        current = temp;
    }
}

bool mll_is_empty(MatrixLinkedList list) {
    return list.first == NULL;
}

Matrix *mll_get(size_t i, MatrixLinkedList list) {
    assert(!mll_is_empty(list));
    assert(i < list.length);

    MatrixNode *current = list.first;
    for (; current != NULL && i > 0; current = current->next) {
        --i;
    }

    if (current != NULL && i == 0) {
        return &current->val;
    }

    return NULL;
}

// If index i is after the bound of the list insert at the end
void mll_insert(size_t i, Matrix mat, MatrixLinkedList *list) {
    assert(i <= list->length);
    list->length++;

    MatrixNode *new_node = (MatrixNode *) malloc(sizeof(MatrixNode));
    matrix_copy(mat, &new_node->val);

    MatrixNode *prev = list->first;
    if (i == 0 || prev == NULL) {
        new_node->next = prev;
        list->first = new_node;
        return;
    }

    for (; prev->next != NULL && i > 0; prev = prev->next) {
        --i;
    }

    new_node->next = prev->next;
    prev->next = new_node;
}

void mll_delete(size_t i, MatrixLinkedList *list) {
    assert(!mll_is_empty(*list));
    assert(i < list->length);
    list->length--;

    MatrixNode *current = list->first;
    if (i == 0) {
        list->first = current->next;
        free(current);
        return;
    }

    MatrixNode *prev = NULL;
    for (; current->next != NULL && i > 0; current = current->next) {
        prev = current;
        --i;
    }

    prev->next = current->next;
    matrix_free(&current->val);
    free(current);
}

void mll_print(MatrixLinkedList list) {
    MatrixNode *current = list.first;

    printf("[\n");
    for (; current != NULL; current = current->next) {
        matrix_print(current->val);
    }
    printf("]\n");
}
