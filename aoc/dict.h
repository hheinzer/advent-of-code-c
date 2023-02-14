/*
 * general purpose dictionary
 */
#ifndef DICT_H
#define DICT_H

#include <stdlib.h>
#include <string.h>

// dict is a collection of items
typedef struct Dict Dict;
typedef struct Item Item;

struct Dict {
    const size_t data_size; // size of data pointed to by item
    size_t len; // number of items in dict
    size_t size; // number of buckets in dict
    Item *item; // bucket array
};

struct Item {
    const char *key; // pointer to item key
    size_t key_size; // key size
    void *data; // pointer to item data
    Item *next; // pointer to next item in bucket
};

// allocate dict
Dict *dict_alloc(size_t data_size, size_t size);

// allocate copy of other,
// use data_copy to copy data from other, if 0 do not copy data
Dict *dict_copy(const Dict *other, void *(*data_copy)(void *, const void *, size_t));

// free dict,
// use data_free to free data, if 0 do not free data
void dict_free(Dict **dict, void (*data_free)(void *));

// insert data into dict with specified key,
// overwrite data if key present,
// return data pointer if key present,
// return 0 if key not present
void *dict_insert(Dict *dict, const char *key, void *data);

// remove item from dict with specified key,
// return data pointer,
// return 0 if key not present
void *dict_remove(Dict *dict, const char *key);

// search for item in dict,
// return item if present,
// return 0 if key not present
Item *dict_find(const Dict *dict, const char *key);

// traverse items of dict and apply func to data,
// return 1 as soon as func returns 1
// return 0 if func always returned 0,
int dict_traverse(const Dict *dict, int (*func)(const char *, void *));

#endif
