#include "list.h"

#include <stdlib.h>

static bool list_ensure_capacity(List *list, size_t minimum_capacity) {
    size_t new_capacity = 0;

    if (!list) {
        return false;
    }

    if (list->capacity >= minimum_capacity) {
        return true;
    }

    new_capacity = (list->capacity == 0) ? 4 : list->capacity;
    while (new_capacity < minimum_capacity) {
        new_capacity *= 2;
    }

    return list_resize(list, new_capacity);
}

List *list_create(size_t initial_capacity) {
    List *list = (List *)calloc(1, sizeof(List));

    if (!list) {
        return NULL;
    }

    if (initial_capacity == 0) {
        initial_capacity = 4;
    }

    list->items = (void **)calloc(initial_capacity, sizeof(void *));
    if (!list->items) {
        free(list);
        return NULL;
    }

    list->capacity = initial_capacity;
    return list;
}

void list_destroy(List *list) {
    if (!list) {
        return;
    }

    free(list->items);
    free(list);
}

bool list_resize(List *list, size_t new_capacity) {
    void **resized_items = NULL;
    size_t i = 0;

    if (!list) {
        return false;
    }

    if (new_capacity == 0) {
        new_capacity = 1;
    }

    resized_items = (void **)realloc(list->items, new_capacity * sizeof(void *));
    if (!resized_items) {
        return false;
    }

    if (new_capacity > list->capacity) {
        for (i = list->capacity; i < new_capacity; ++i) {
            resized_items[i] = NULL;
        }
    }

    if (list->size > new_capacity) {
        list->size = new_capacity;
    }

    list->items = resized_items;
    list->capacity = new_capacity;
    return true;
}

bool list_push_back(List *list, void *item) {
    if (!list_ensure_capacity(list, list ? list->size + 1 : 0)) {
        return false;
    }

    list->items[list->size++] = item;
    return true;
}

bool list_insert(List *list, size_t index, void *item) {
    size_t i = 0;

    if (!list || index > list->size) {
        return false;
    }

    if (!list_ensure_capacity(list, list->size + 1)) {
        return false;
    }

    for (i = list->size; i > index; --i) {
        list->items[i] = list->items[i - 1];
    }

    list->items[index] = item;
    list->size++;
    return true;
}

bool list_set(List *list, size_t index, void *item) {
    if (!list || index >= list->size) {
        return false;
    }

    list->items[index] = item;
    return true;
}

void *list_get(const List *list, size_t index) {
    if (!list || index >= list->size) {
        return NULL;
    }

    return list->items[index];
}

void *list_pop_back(List *list) {
    void *item = NULL;

    if (!list || list->size == 0) {
        return NULL;
    }

    item = list->items[list->size - 1];
    list->items[list->size - 1] = NULL;
    list->size--;
    return item;
}

void *list_remove_at(List *list, size_t index) {
    void *item = NULL;
    size_t i = 0;

    if (!list || index >= list->size) {
        return NULL;
    }

    item = list->items[index];

    for (i = index; i + 1 < list->size; ++i) {
        list->items[i] = list->items[i + 1];
    }

    list->items[list->size - 1] = NULL;
    list->size--;
    return item;
}

void list_clear(List *list) {
    size_t i = 0;

    if (!list) {
        return;
    }

    for (i = 0; i < list->size; ++i) {
        list->items[i] = NULL;
    }

    list->size = 0;
}

bool list_is_empty(const List *list) {
    return !list || list->size == 0;
}
