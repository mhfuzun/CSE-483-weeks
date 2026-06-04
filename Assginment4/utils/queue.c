#include "queue.h"

#include <stdlib.h>

Queue *queue_create(void) {
    Queue *queue = (Queue *)calloc(1, sizeof(Queue));

    if (!queue) {
        return NULL;
    }

    queue->storage = linked_list_create();
    if (!queue->storage) {
        free(queue);
        return NULL;
    }

    return queue;
}

void queue_destroy(Queue *queue) {
    if (!queue) {
        return;
    }

    linked_list_destroy(queue->storage);
    free(queue);
}

bool queue_enqueue(Queue *queue, void *data) {
    if (!queue || !queue->storage) {
        return false;
    }

    return linked_list_push_back(queue->storage, data);
}

void *queue_dequeue(Queue *queue) {
    if (!queue || !queue->storage) {
        return NULL;
    }

    return linked_list_pop_front(queue->storage);
}

void *queue_peek(const Queue *queue) {
    if (!queue || !queue->storage) {
        return NULL;
    }

    return linked_list_peek_front(queue->storage);
}

bool queue_is_empty(const Queue *queue) {
    return !queue || !queue->storage || linked_list_is_empty(queue->storage);
}

size_t queue_size(const Queue *queue) {
    if (!queue || !queue->storage) {
        return 0;
    }

    return queue->storage->size;
}
