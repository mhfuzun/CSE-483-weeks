#ifndef UTILS_STACK_H
#define UTILS_STACK_H

#include <stdbool.h>
#include <stddef.h>

#include "list.h"

typedef struct {
    List *storage;
} Stack;

Stack *stack_create(size_t initial_capacity);
void stack_destroy(Stack *stack);

bool stack_push(Stack *stack, void *data);
void *stack_pop(Stack *stack);
void *stack_peek(const Stack *stack);

bool stack_is_empty(const Stack *stack);
size_t stack_size(const Stack *stack);

#endif
