#include "htable.h"

Htable *htable_create(size_t data_size, size_t table_size)
{
    Htable *htable = malloc(sizeof(*htable));
    memcpy(htable, &(Htable) { .data_size = data_size }, sizeof(*htable));
    htable->table_size = table_size;
    htable->elem = calloc(table_size, sizeof(*htable->elem));
    return htable;
}

void htable_free(Htable *htable)
{
    for (size_t i = 0; i < htable->table_size; ++i) {
        HtableElement *elem = htable->elem[i];
        while (elem) {
            HtableElement *next = elem->next;
            free(elem->key);
            free(elem->data);
            free(elem);
            elem = next;
        }
    }
    free(htable->elem);
    free(htable);
}

size_t htable_hash(const char *key)
{
    // djb2 (http://www.cse.yorku.ca/~oz/hash.html)
    size_t hash = 5381;
    int c;
    while ((c = *(key++))) {
        hash = ((hash << 5) + hash) + c; // hash * 33 + c
    }
    return hash;
}

void htable_insert(Htable *htable, const char *key, void *data)
{
    // get insertion element
    const size_t ielem = htable_hash(key) % htable->table_size;
    HtableElement *elem = htable->elem[ielem];

    HtableElement *prev = 0;
    while (elem && strcmp(elem->key, key)) {
        prev = elem;
        elem = elem->next;
    }

    if (!elem) {
        // create new element
        elem = calloc(1, sizeof(*elem));
        elem->key = malloc(strlen(key) + 1);
        strcpy(elem->key, key);
        elem->data = data;
        ++htable->nelem;

        // update chain
        if (prev) {
            prev->next = elem;
        } else {
            htable->elem[ielem] = elem;
        }

    } else {
        // same key: exchange data
        free(elem->data);
        elem->data = data;
    }
}

void htable_insert_copy(Htable *htable, const char *key, const void *data, void *(*data_copy)(void *, const void *, size_t))
{
    void *copy = malloc(htable->data_size);
    data_copy(copy, data, htable->data_size);
    htable_insert(htable, key, copy);
}

void *htable_remove(Htable *htable, const char *key)
{
    const size_t ielem = htable_hash(key) % htable->table_size;
    HtableElement *elem = htable->elem[ielem];

    HtableElement *prev = 0;
    while (elem && strcmp(elem->key, key)) {
        prev = elem;
        elem = elem->next;
    }

    if (!elem) {
        // element is not in hash table
        return 0;

    } else {
        // get data pointer and delete element
        void *data = elem->data;
        HtableElement *next = elem->next;
        free(elem->key);
        free(elem);
        --htable->nelem;

        // update chain
        if (prev) {
            prev->next = next;
        } else {
            htable->elem[ielem] = 0;
        }

        return data;
    }
}

HtableElement *htable_search(const Htable *htable, const char *key)
{
    const size_t ielem = htable_hash(key) % htable->table_size;
    HtableElement *elem = htable->elem[ielem];

    while (elem && elem->key && strcmp(elem->key, key)) {
        elem = elem->next;
    }

    return elem;
}
