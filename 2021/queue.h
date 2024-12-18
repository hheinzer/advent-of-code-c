#pragma once

#include "list.h"

typedef List Queue;

static Queue *queue_alloc(size_t data_size)
{
    return list_alloc(data_size);
}

static void queue_free(Queue **queue, void (*data_free)(void *))
{
    list_free(queue, data_free);
}

static int queue_push(Queue *queue, void *data)
{
    return list_insert_last(queue, data);
}

static void *queue_pop(Queue *queue)
{
    return list_remove_first(queue);
}
