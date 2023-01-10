#include "list.h"

static Node *node_alloc(void *data)
{
    const Node node = { .data = data };
    return memcpy(malloc(sizeof(node)), &node, sizeof(node));
}

static void node_free(Node **node, void (*data_free)(void *))
{
    if (data_free) {
        data_free((*node)->data);
        (*node)->data = 0;
    }
    free(*node);
    *node = 0;
}

List *list_alloc(size_t data_size)
{
    const List list = { .data_size = data_size };
    return memcpy(malloc(sizeof(list)), &list, sizeof(list));
}

void list_free(List **list, void (*data_free)(void *))
{
    Node *node = (*list)->head;
    while (node) {
        Node *next = node->next;
        node_free(&node, data_free);
        node = next;
    }
    free(*list);
    *list = 0;
}

int list_insert(List *list, size_t i, void *data)
{
    if (i > list->len) {
        return 1; // index out of range
    }
    Node *node = node_alloc(data);
    if (list->len == 0) { // empty list
        list->head = node;
        list->tail = node;

    } else if (i == 0) { // insert at head
        node->next = list->head;
        list->head->prev = node;
        list->head = node;

    } else if (i == list->len) { // insert at tail
        node->prev = list->tail;
        list->tail->next = node;
        list->tail = node;

    } else { // insert in middle
        Node *next = 0;
        if (i <= list->len - 1 - i) { // search forward
            next = list->head;
            for (size_t j = 0; j < i; ++j) {
                next = next->next;
            }
        } else { // search backward
            next = list->tail;
            for (size_t j = 0; j < list->len - 1 - i; ++j) {
                next = next->prev;
            }
        }
        node->next = next;
        node->prev = next->prev;
        node->next->prev = node;
        node->prev->next = node;
    }
    ++list->len; // increment length
    return 0;
}

int list_insert_front(List *list, void *data)
{
    return list_insert(list, 0, data);
}

int list_insert_back(List *list, void *data)
{
    return list_insert(list, list->len, data);
}

void *list_remove(List *list, size_t i)
{
    if (i >= list->len) {
        return 0; // index out of range
    }
    Node *node = 0;
    if (list->len == 1) { // remove only node
        node = list->head;
        list->head = 0;
        list->tail = 0;

    } else if (i == 0) { // remove head
        node = list->head;
        list->head = node->next;
        list->head->prev = 0;

    } else if (i == list->len - 1) { // remove tail
        node = list->tail;
        list->tail = node->prev;
        list->tail->next = 0;

    } else { // remove in middle
        if (i <= list->len - 1 - i) { // search forward
            node = list->head;
            for (size_t j = 0; j < i; ++j) {
                node = node->next;
            }
        } else { // search backward
            node = list->tail;
            for (size_t j = 0; j < list->len - 1 - i; ++j) {
                node = node->prev;
            }
        }
        node->next->prev = node->prev;
        node->prev->next = node->next;
    }
    void *data = node->data;
    node_free(&node, 0);
    --list->len; // decrement length
    return data;
}

void *list_remove_front(List *list)
{
    return list_remove(list, 0);
}

void *list_remove_back(List *list)
{
    return list_remove(list, list->len - 1);
}

void list_sort(List *list, int (*data_cmp)(const void *, const void *))
{
    char *data = malloc(list->len * list->data_size);
    Node *node = list->head;
    for (size_t i = 0; i < list->len; ++i) { // shallow copy to data array
        memcpy(data + i * list->data_size, node->data, list->data_size);
        node = node->next;
    }
    qsort(data, list->len, list->data_size, data_cmp);
    node = list->head;
    for (size_t i = 0; i < list->len; ++i) { // shallow copy to list
        memcpy(node->data, data + i * list->data_size, list->data_size);
        node = node->next;
    }
    free(data);
}

Node *list_find(const List *list, void *data, int (*data_cmp)(const void *, const void *))
{
    Node *node = list->head;
    while (node) {
        if (!data_cmp(node->data, data)) {
            return node;
        }
        node = node->next;
    }
    return 0;
}

size_t list_index(const List *list, void *data, int (*data_cmp)(const void *, const void *))
{
    Node *node = list->head;
    for (size_t i = 0; i < list->len; ++i) {
        if (!data_cmp(node->data, data)) {
            return i;
        }
        node = node->next;
    }
    return list->len;
}
