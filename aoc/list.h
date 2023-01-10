#ifndef LIST_H
#define LIST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct List List;
typedef struct Node Node;

struct List {
    const size_t data_size; // size of data pointed to by node
    size_t len; // length of list
    Node *head; // head of list
    Node *tail; // tail of list
};

struct Node {
    void *data; // pointer to node data
    Node *next; // pointer to next node
    Node *prev; // pointer to previous node
};

// allocate list
List *list_alloc(size_t data_size);

// free list, use data_free to free data in node, use 0 to not free data
void list_free(List **list, void (*data_free)(void *));

// insert data into list at specified location,
// return 0 for success and 1 for failure
int list_insert(List *list, size_t i, void *data);
int list_insert_front(List *list, void *data);
int list_insert_back(List *list, void *data);

// remove node from list at specified location, return data pointer,
// return 0 if i out of range
void *list_remove(List *list, size_t i);
void *list_remove_front(List *list);
void *list_remove_back(List *list);

// sort list in place
void list_sort(List *list, int (*data_cmp)(const void *, const void *));

// search for first occurrence of data in list, if found return matching node,
// return 0 if not found
Node *list_find(const List *list, void *data, int (*data_cmp)(const void *, const void *));

// search for first occurrence of data in list, if found return index of matching node,
// return list->len (out of range) if not found
size_t list_index(const List *list, void *data, int (*data_cmp)(const void *, const void *));

#endif