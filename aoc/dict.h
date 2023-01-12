/*
 * general purpose dictionary
 */
#ifndef DICT_H
#define DICT_H

#include <stdlib.h>
#include <string.h>

typedef struct Dict Dict;
typedef struct Item Item;

struct Dict {
    const size_t data_size; // size of data pointed to by item
    size_t len; // number of items in dict
    size_t size; // number of buckets in dict
    Item *item; // bucket array
};

struct Item {
    char *key; // pointer to item key
    void *data; // pointer to item data
    Item *next; // pointer to next item in bucket
};

// allocate dict
Dict *dict_alloc(size_t data_size, size_t size);

// free dict, use data_free to free data in item, use 0 to not free data
void dict_free(Dict **dict, void (*data_free)());

// insert data into dict with specified key,
// if key already present, overwrite data and return old data pointer
// return 0 if key not present
void *dict_insert(Dict *dict, const char *key, void *data);

// remove item from dict with specified key, return data pointer,
// return 0 if key not present
void *dict_remove(Dict *dict, const char *key);

// search for item in dict, if present return it,
// return 0 if key not present
Item *dict_find(Dict *dict, const char *key);

#endif
