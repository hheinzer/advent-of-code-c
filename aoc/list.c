#include "list.h"

static Node *node_alloc(void *data)
{
    const Node node = { .data = data };
    return memdup(&node, sizeof(node));
}

static void node_free(Node *node, void (*data_free)(void *))
{
    if (data_free) {
        data_free(node->data);
        node->data = 0;
    }
    free(node);
}

List *list_alloc(size_t data_size)
{
    const List list = { .data_size = data_size };
    return memdup(&list, sizeof(list));
}

List *list_copy(const List *other, void *(*data_copy)(void *, const void *, size_t))
{
    List *list = list_alloc(other->data_size);
    for (const Node *node = other->first; node; node = node->next) {
        void *copy = 0;
        if (data_copy) {
            copy = data_copy(malloc(list->data_size), node->data, list->data_size);
        } else {
            copy = node->data;
        }
        list_insert_last(list, copy);
    }
    return list;
}

void list_free(List **list, void (*data_free)(void *))
{
    Node *node = (*list)->first;
    for (size_t i = 0; i < (*list)->len; ++i) {
        Node *next = node->next;
        node_free(node, data_free);
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
        list->first = node;
        list->last = node;

    } else if (i == 0) { // insert at first node
        node->next = list->first;
        list->first->prev = node;
        list->first = node;

    } else if (i == list->len) { // insert at last node
        node->prev = list->last;
        list->last->next = node;
        list->last = node;

    } else { // insert in middle
        Node *next = 0;
        if (i <= list->len - 1 - i) { // search forward
            next = list->first;
            for (size_t j = 0; j < i; ++j) { next = next->next; }
        } else { // search backward
            next = list->last;
            for (size_t j = 0; j < list->len - 1 - i; ++j) { next = next->prev; }
        }
        node->next = next;
        node->prev = next->prev;
        node->next->prev = node;
        node->prev->next = node;
    }
    ++list->len; // increment length
    return 0;
}

int list_insert_first(List *list, void *data) { return list_insert(list, 0, data); }

int list_insert_last(List *list, void *data)
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
        node = list->first;
        list->first = 0;
        list->last = 0;

    } else if (i == 0) { // remove first node
        node = list->first;
        list->first = node->next;
        list->first->prev = 0;

    } else if (i == list->len - 1) { // remove last node
        node = list->last;
        list->last = node->prev;
        list->last->next = 0;

    } else { // remove in middle
        if (i <= list->len - 1 - i) { // search forward
            node = list->first;
            for (size_t j = 0; j < i; ++j) { node = node->next; }
        } else { // search backward
            node = list->last;
            for (size_t j = 0; j < list->len - 1 - i; ++j) { node = node->prev; }
        }
        node->next->prev = node->prev;
        node->prev->next = node->next;
    }
    void *data = node->data;
    node_free(node, 0);
    --list->len; // decrement length
    return data;
}

void *list_remove_first(List *list) { return list_remove(list, 0); }

void *list_remove_last(List *list) { return list_remove(list, list->len - 1); }

Node *list_get(const List *list, size_t i)
{
    if (i >= list->len) {
        return 0; // index out of range
    }
    if (i == 0) { // return first node
        return list->first;

    } else if (i == list->len - 1) { // return last node
        return list->last;

    } else { // return in middle node
        Node *node = 0;
        if (i <= list->len - 1 - i) { // search forward
            node = list->first;
            for (size_t j = 0; j < i; ++j) { node = node->next; }
        } else { // search backward
            node = list->last;
            for (size_t j = 0; j < list->len - 1 - i; ++j) { node = node->prev; }
        }
        return node;
    }
}

Node *list_find(
    const List *list, const void *data, int (*data_cmp)(const void *, const void *))
{
    for (Node *node = list->first; node; node = node->next) {
        if (!data_cmp(node->data, data)) { return node; }
    }
    return 0;
}

void *list_delete(List *list, Node *node)
{
    if (node == list->first) { // delete first node
        list->first = list->first->next;
        list->first->prev = 0;

    } else if (node == list->last) { // delete last node
        list->last = list->last->prev;
        list->last->next = 0;

    } else { // delete middle node
        node->next->prev = node->prev;
        node->prev->next = node->next;
    }
    void *data = node->data;
    node_free(node, 0);
    --list->len; // decrement length
    return data;
}

size_t list_index(
    const List *list, void *data, int (*data_cmp)(const void *, const void *))
{
    Node *node = list->first;
    for (size_t i = 0; i < list->len; ++i) {
        if (!data_cmp(node->data, data)) { return i; }
        node = node->next;
    }
    return list->len;
}

void list_sort(List *list, int (*data_cmp)(const void *, const void *))
{
    char *data = list_to_array(list);
    qsort(data, list->len, list->data_size, data_cmp);
    Node *node = list->first;
    for (size_t i = 0; i < list->len; ++i) { // shallow copy to list
        memcpy(node->data, data + i * list->data_size, list->data_size);
        node = node->next;
    }
    free(data);
}

void *list_to_array(const List *list)
{
    char *data = malloc(list->len * list->data_size);
    Node *node = list->first;
    for (size_t i = 0; i < list->len; ++i) { // shallow copy to data array
        memcpy(data + i * list->data_size, node->data, list->data_size);
        node = node->next;
    }
    return data;
}
