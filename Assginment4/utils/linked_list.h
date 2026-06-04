#ifndef UTILS_LINKED_LIST_H
#define UTILS_LINKED_LIST_H

#include <stdbool.h>
#include <stddef.h>

typedef struct LinkedListNode LinkedListNode;

struct LinkedListNode {
    void *data;
    LinkedListNode *next;
    LinkedListNode *prev;
};

typedef struct {
    LinkedListNode *head;
    LinkedListNode *tail;
    size_t size;
} LinkedList;

LinkedList *linked_list_create(void);
void linked_list_destroy(LinkedList *list);

bool linked_list_push_front(LinkedList *list, void *data);
bool linked_list_push_back(LinkedList *list, void *data);

void *linked_list_pop_front(LinkedList *list);
void *linked_list_pop_back(LinkedList *list);
void *linked_list_peek_front(const LinkedList *list);
void *linked_list_peek_back(const LinkedList *list);

bool linked_list_is_empty(const LinkedList *list);
void linked_list_clear(LinkedList *list);

#endif
