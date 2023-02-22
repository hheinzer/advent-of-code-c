#include "heap.h"

Heap *heap_alloc(size_t data_size, size_t size)
{
    size = (size ? size : 1);
    const Heap heap = {
        .data_size = data_size,
        .size = size,
        .elem = calloc(size, sizeof(Elem)),
    };
    return memdup(&heap, sizeof(heap));
}

Heap *heap_copy(const Heap *other, void *(*data_copy)(void *, const void *, size_t))
{
    Heap *heap = heap_alloc(other->data_size, other->size);
    heap->len = other->len;
    for (size_t i = 0; i < heap->len; ++i) {
        void *copy = 0;
        if (data_copy) {
            copy = data_copy(malloc(heap->data_size), other->elem[i].data, heap->data_size);
        } else {
            copy = other->elem[i].data;
        }
        heap_insert(heap, other->elem[i].priority, copy);
    }
    return heap;
}

void heap_free(Heap **heap, void (*data_free)(void *))
{
    if (data_free) {
        for (size_t i = 0; i < (*heap)->len; ++i) {
            data_free((*heap)->elem[i].data);
        }
    }
    free((*heap)->elem);
    free(*heap);
    *heap = 0;
}

static size_t parent(size_t i)
{
    return (i - 1) / 2;
}

static void elem_swap(Elem *a, Elem *b)
{
    long swap_priority = a->priority;
    a->priority = b->priority;
    b->priority = swap_priority;
    void *swap_data = a->data;
    a->data = b->data;
    b->data = swap_data;
}

void heap_insert(Heap *heap, long priority, void *data)
{
    if (heap->len == heap->size) { // grow heap if necessary
        heap->size *= 2;
        heap->elem = realloc(heap->elem, heap->size * sizeof(Elem));
    }
    size_t i = heap->len++;
    Elem *elem = heap->elem;
    elem[i].priority = priority;
    elem[i].data = data;
    while ((i != 0) && (elem[parent(i)].priority < elem[i].priority)) { // heapify
        elem_swap(&elem[parent(i)], &elem[i]);
        i = parent(i);
    }
}

static size_t left(size_t i)
{
    return 2 * i + 1;
}

static size_t right(size_t i)
{
    return 2 * i + 2;
}

void heapify(Heap *heap, size_t i)
{
    const size_t l = left(i);
    const size_t r = right(i);
    size_t max = i;
    Elem *elem = heap->elem;
    if ((l < heap->len) && (elem[l].priority > elem[i].priority)) {
        max = l;
    }
    if ((r < heap->len) && (elem[r].priority > elem[max].priority)) {
        max = r;
    }
    if (max != i) {
        elem_swap(&elem[i], &elem[max]);
        heapify(heap, max);
    }
}

void *heap_remove(Heap *heap)
{
    if (heap->len == 0) {
        return 0;
    }
    --heap->len;
    Elem *elem = heap->elem;
    void *data = elem[0].data;
    elem_swap(&elem[0], &elem[heap->len]);
    if (heap->len > 1) {
        heapify(heap, 0);
    }
    return data;
}

Elem *heap_peek(Heap *heap)
{
    if (heap->len == 0) {
        return 0;
    }
    return &heap->elem[0];
}
