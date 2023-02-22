/*
 * general purpose binary heap
 */
#ifndef HEAP_H
#define HEAP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "memory.h"

// heap is a complete binary tree of elements
typedef struct Heap Heap;
typedef struct Elem Elem;

struct Heap {
    const size_t data_size; // size of data pointed to by element
    size_t len; // length of list
    size_t size; // maximum length of list
    Elem *elem; // element array
};

struct Elem {
    long priority; // priority of element
    void *data; // pointer to element data
};

// allocate heap,
// return 0 on memory failure
Heap *heap_alloc(size_t data_size, size_t size);

// allocate copy of heap,
// use data_copy to copy data from other, if 0 do not copy data,
// return 0 on memory failure
Heap *heap_copy(const Heap *other, void *(*data_copy)(void *, const void *, size_t));

// free heap,
// use data_free to free data, if 0 do not free data
void heap_free(Heap **heap, void (*data_free)(void *));

// insert data into heap with specified priority,
// return 0 for success,
// return 1 for failure
int heap_insert(Heap *heap, long priority, void *data);

// remove highest priority element from heap,
// return 0, if heap is empty
void *heap_remove(Heap *heap);

// return pointer to highest priority element in heap,
// return 0, if heap is empty
Elem *heap_peek(Heap *heap);

#endif
