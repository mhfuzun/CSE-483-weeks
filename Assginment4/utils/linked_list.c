#include "linked_list.h"

#include <stdlib.h>

static LinkedListNode *linked_list_node_create(void *data) {
    LinkedListNode *node = (LinkedListNode *)calloc(1, sizeof(LinkedListNode));

    if (!node) {
        return NULL;
    }

    node->data = data;
    return node;
}

LinkedList *linked_list_create(void) {
    return (LinkedList *)calloc(1, sizeof(LinkedList));
}

void linked_list_clear(LinkedList *list) {
    LinkedListNode *node = NULL;
    LinkedListNode *next = NULL;

    if (!list) {
        return;
    }

    node = list->head;
    while (node) {
        next = node->next;
        free(node);
        node = next;
    }

    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
}

void linked_list_destroy(LinkedList *list) {
    if (!list) {
        return;
    }

    linked_list_clear(list);
    free(list);
}

bool linked_list_push_front(LinkedList *list, void *data) {
    LinkedListNode *node = NULL;

    if (!list) {
        return false;
    }

    node = linked_list_node_create(data);
    if (!node) {
        return false;
    }

    node->next = list->head;
    if (list->head) {
        list->head->prev = node;
    } else {
        list->tail = node;
    }

    list->head = node;
    list->size++;
    return true;
}

bool linked_list_push_back(LinkedList *list, void *data) {
    LinkedListNode *node = NULL;

    if (!list) {
        return false;
    }

    node = linked_list_node_create(data);
    if (!node) {
        return false;
    }

    node->prev = list->tail;
    if (list->tail) {
        list->tail->next = node;
    } else {
        list->head = node;
    }

    list->tail = node;
    list->size++;
    return true;
}

void *linked_list_pop_front(LinkedList *list) {
    LinkedListNode *node = NULL;
    void *data = NULL;

    if (!list || !list->head) {
        return NULL;
    }

    node = list->head;
    data = node->data;

    list->head = node->next;
    if (list->head) {
        list->head->prev = NULL;
    } else {
        list->tail = NULL;
    }

    free(node);
    list->size--;
    return data;
}

void *linked_list_pop_back(LinkedList *list) {
    LinkedListNode *node = NULL;
    void *data = NULL;

    if (!list || !list->tail) {
        return NULL;
    }

    node = list->tail;
    data = node->data;

    list->tail = node->prev;
    if (list->tail) {
        list->tail->next = NULL;
    } else {
        list->head = NULL;
    }

    free(node);
    list->size--;
    return data;
}

void *linked_list_peek_front(const LinkedList *list) {
    if (!list || !list->head) {
        return NULL;
    }

    return list->head->data;
}

void *linked_list_peek_back(const LinkedList *list) {
    if (!list || !list->tail) {
        return NULL;
    }

    return list->tail->data;
}

bool linked_list_is_empty(const LinkedList *list) {
    return !list || list->size == 0;
}
