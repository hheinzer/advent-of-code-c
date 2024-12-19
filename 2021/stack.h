#pragma once

#include "list.h"

typedef List Stack;

Stack *stack_alloc(size_t data_size)
{
    return list_alloc(data_size);
}

void stack_free(Stack **stack, void (*data_free)(void *))
{
    list_free(stack, data_free);
}

int stack_push(Stack *stack, void *data)
{
    return list_insert_last(stack, data);
}

void *stack_pop(Stack *stack)
{
    return list_remove_last(stack);
}
