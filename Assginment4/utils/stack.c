#include "stack.h"

#include <stdlib.h>

Stack *stack_create(size_t initial_capacity) {
    Stack *stack = (Stack *)calloc(1, sizeof(Stack));

    if (!stack) {
        return NULL;
    }

    stack->storage = list_create(initial_capacity);
    if (!stack->storage) {
        free(stack);
        return NULL;
    }

    return stack;
}

void stack_destroy(Stack *stack) {
    if (!stack) {
        return;
    }

    list_destroy(stack->storage);
    free(stack);
}

bool stack_push(Stack *stack, void *data) {
    if (!stack || !stack->storage) {
        return false;
    }

    return list_push_back(stack->storage, data);
}

void *stack_pop(Stack *stack) {
    if (!stack || !stack->storage) {
        return NULL;
    }

    return list_pop_back(stack->storage);
}

void *stack_peek(const Stack *stack) {
    if (!stack || !stack->storage || stack->storage->size == 0) {
        return NULL;
    }

    return list_get(stack->storage, stack->storage->size - 1);
}

bool stack_is_empty(const Stack *stack) {
    return !stack || !stack->storage || list_is_empty(stack->storage);
}

size_t stack_size(const Stack *stack) {
    if (!stack || !stack->storage) {
        return 0;
    }

    return stack->storage->size;
}
