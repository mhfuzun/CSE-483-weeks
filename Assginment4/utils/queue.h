#ifndef UTILS_QUEUE_H
#define UTILS_QUEUE_H

#include <stdbool.h>
#include <stddef.h>

#include "linked_list.h"

typedef struct {
    LinkedList *storage;
} Queue;

Queue *queue_create(void);
void queue_destroy(Queue *queue);

bool queue_enqueue(Queue *queue, void *data);
void *queue_dequeue(Queue *queue);
void *queue_peek(const Queue *queue);

bool queue_is_empty(const Queue *queue);
size_t queue_size(const Queue *queue);

#endif
