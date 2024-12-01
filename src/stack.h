/*
 * general purpose stack
 */
#ifndef STACK_H
#define STACK_H

#include "list.h"

typedef List Stack;

inline Stack *stack_alloc(size_t data_size)
{
    return list_alloc(data_size);
}

inline void stack_free(Stack **stack, void (*data_free)(void *))
{
    list_free(stack, data_free);
}

inline int stack_push(Stack *stack, void *data)
{
    return list_insert_last(stack, data);
}

inline void *stack_pop(Stack *stack)
{
    return list_remove_last(stack);
}

#endif
