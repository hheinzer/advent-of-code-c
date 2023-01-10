#include "stack.h"

Stack *stack_alloc(size_t data_size);

void stack_free(Stack **stack, void (*data_free)());

int stack_push(Stack *stack, void *data);

void *stack_pop(Stack *stack);
