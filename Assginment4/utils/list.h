#ifndef UTILS_LIST_H
#define UTILS_LIST_H

#include <stdbool.h>
#include <stddef.h>

typedef struct {
    void **items;
    size_t size;
    size_t capacity;
} List;

List *list_create(size_t initial_capacity);
void list_destroy(List *list);

bool list_resize(List *list, size_t new_capacity);
bool list_push_back(List *list, void *item);
bool list_insert(List *list, size_t index, void *item);
bool list_set(List *list, size_t index, void *item);

void *list_get(const List *list, size_t index);
void *list_pop_back(List *list);
void *list_remove_at(List *list, size_t index);

void list_clear(List *list);
bool list_is_empty(const List *list);

#endif
