#include "queue.h"

Queue *queue_alloc(size_t data_size);

void queue_free(Queue **queue, void (*data_free)());

int queue_push(Queue *queue, void *data);

void *queue_pop(Queue *queue);
