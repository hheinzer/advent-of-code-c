/*
 * general purpose hash table
 */
#ifndef HTABLE_H
#define HTABLE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Htable Htable;
typedef struct HtableElement HtableElement;

struct Htable {
    const size_t data_size;
    size_t table_size;
    size_t nelem;
    HtableElement **elem;
};

struct HtableElement {
    char *key;
    void *data;
    HtableElement *next;
};

// create hash table
Htable *htable_create(size_t data_size, size_t table_size);

// free hash table
void htable_free(Htable *htable);

// hash function
size_t htable_hash(const char *key);

// insert element into hash table, no copying of data
void htable_insert(Htable *htable, const char *key, void *data);

// insert element into hash table, copy data
void htable_insert_copy(Htable *htable, const char *key, const void *data, void *(*data_copy)(void *, const void *, size_t));

// remove element from hash table, return its data pointer
void *htable_remove(Htable *htable, const char *key);

// search element in hash table, return its data pointer, return 0 if it doesn't exist
HtableElement *htable_search(const Htable *htable, const char *key);

#endif
