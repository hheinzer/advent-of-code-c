/*
 * general purpose queue
 */
#ifndef QUEUE_H
#define QUEUE_H

#include "list.h"

typedef List Queue;

inline Queue *queue_alloc(size_t data_size)
{
    return list_alloc(data_size);
}

inline void queue_free(Queue **queue, void (*data_free)())
{
    list_free(queue, data_free);
}

inline int queue_push(Queue *queue, void *data)
{
    return list_insert_last(queue, data);
}

inline void *queue_pop(Queue *queue)
{
    return list_remove_first(queue);
}

#endif
