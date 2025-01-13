#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "memory.h"

// dict is a collection of items
typedef struct Dict Dict;
typedef struct Item Item;

struct Dict {
    const size_t data_size;  // size of data pointed to by item
    size_t len;              // number of items in dict
    size_t size;             // number of buckets in dict
    Item *item;              // bucket array
};

struct Item {
    const char *key;  // pointer to item key
    size_t key_size;  // key size
    void *data;       // pointer to item data
    Item *next;       // pointer to next item in bucket
};

Item *_item_alloc(const char *key, size_t key_size, void *data) {
    const Item item = {
        .key = memdup(key, key_size + 1),
        .key_size = key_size,
        .data = data,
    };
    return memdup(&item, sizeof(item));
}

void _item_clear(Item *item, void (*data_free)(void *)) {
    free((void *)item->key);
    item->key = 0;
    item->key_size = 0;
    if (data_free) {
        data_free(item->data);
        item->data = 0;
    }
    item->next = 0;
}

void _item_free(Item *item, void (*data_free)(void *)) {
    _item_clear(item, data_free);
    free(item);
}

// allocate dict
Dict *dict_alloc(size_t data_size, size_t size) {
    const Dict dict = {
        .data_size = data_size,
        .size = size,
        .item = calloc(size, sizeof(Item)),
    };
    return memdup(&dict, sizeof(dict));
}

size_t _hash(const char *str) {
    // source: https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function
    size_t hash = 14695981039346656037ULL;
    unsigned char c;
    while ((c = *str++)) {
        hash ^= c;
        hash *= 1099511628211ULL;
    }
    return hash;
}

int _keycmp(const char *key1, size_t key_size1, const char *key2, size_t key_size2) {
    if (key_size1 != key_size2) {
        return 1;
    }
    while (*key1 && (*key1 == *key2)) {
        ++key1;
        ++key2;
    }
    return !(*key1 == *key2);
}

// insert data into dict with specified key,
// overwrite data if key present,
// return data pointer if key present,
// return 0 if key not present
void *dict_insert(Dict *dict, const char *key, void *data) {
    const size_t key_size = strlen(key);
    const size_t i = _hash(key) % dict->size;
    Item *item = &dict->item[i];
    Item *prev = item;
    while (item && item->key && _keycmp(item->key, item->key_size, key, key_size)) {
        prev = item;
        item = item->next;
    }
    if (!item) {  // collision: append item
        item = _item_alloc(key, key_size, data);
        prev->next = item;
        ++dict->len;
        return 0;
    }
    if (!item->key) {  // empty spot: insert item
        item->key = memdup(key, key_size + 1);
        item->key_size = key_size;
        item->data = data;
        ++dict->len;
        return 0;
    }
    // same key: update data
    void *item_data = item->data;
    item->data = data;
    return item_data;
}

// allocate copy of other,
// use data_copy to copy data from other, if 0 do not copy data
Dict *dict_copy(const Dict *other, void *(*data_copy)(void *, const void *, size_t)) {
    Dict *dict = dict_alloc(other->data_size, other->size);
    for (size_t i = 0; i < other->size; ++i) {
        for (const Item *item = &other->item[i]; item && item->key; item = item->next) {
            void *copy = 0;
            if (data_copy) {
                copy = data_copy(malloc(dict->data_size), item->data, dict->data_size);
            }
            else {
                copy = item->data;
            }
            dict_insert(dict, item->key, copy);
        }
    }
    return dict;
}

// free dict,
// use data_free to free data, if 0 do not free data
void dict_free(Dict **dict, void (*data_free)(void *)) {
    for (size_t i = 0; i < (*dict)->size; ++i) {
        Item *item = &(*dict)->item[i];
        if (item->key) {
            Item *next = item->next;
            _item_clear(item, data_free);
            item = next;
            while (item) {
                next = item->next;
                _item_free(item, data_free);
                item = next;
            }
        }
    }
    free((*dict)->item);
    free(*dict);
    *dict = 0;
}

// remove item from dict with specified key,
// return data pointer,
// return 0 if key not present
void *dict_remove(Dict *dict, const char *key) {
    const size_t key_size = strlen(key);
    const size_t i = _hash(key) % dict->size;
    Item *item = &dict->item[i];
    Item *prev = 0;
    while (item && item->key && _keycmp(item->key, item->key_size, key, key_size)) {
        prev = item;
        item = item->next;
    }
    if (!item || !item->key) {  // item not in dict
        return 0;
    }
    // delete item, return data pointer
    void *data = item->data;
    Item *next = item->next;
    if (prev) {  // chained item: free and update chain
        _item_free(item, 0);
        prev->next = next;
    }
    else {  // head item: clear and move chain up
        _item_clear(item, 0);
        if (next) {
            *item = *next;
            free(next);
        }
    }
    --dict->len;
    return data;
}

// search for item in dict,
// return item if present,
// return 0 if key not present
Item *dict_find(const Dict *dict, const char *key) {
    const size_t key_size = strlen(key);
    const size_t i = _hash(key) % dict->size;
    Item *item = &dict->item[i];
    while (item && item->key && _keycmp(item->key, item->key_size, key, key_size)) {
        item = item->next;
    }
    return (!item || !item->key ? 0 : item);
}

int _cmp_size_t_asc(const void *a, const void *b) {
    return (*(size_t *)a > *(size_t *)b) - (*(size_t *)a < *(size_t *)b);
}

// print collision histogram of dict
void dict_histogram(const Dict *dict) {
    // compute occupation histogram of dict
    char key[256] = "";
    Dict *hist = dict_alloc(sizeof(size_t), dict->len);
    for (size_t i = 0; i < dict->size; ++i) {
        size_t n = 0;
        for (const Item *item = &dict->item[i]; item && item->key; item = item->next) ++n;
        snprintf(key, sizeof(key), "%zu", n);
        Item *item_hist = dict_find(hist, key);
        if (item_hist) {
            ++(*(size_t *)item_hist->data);
        }
        else {
            dict_insert(hist, key, memdup((size_t[1]){1}, sizeof(size_t[1])));
        }
    }

    // create sorted array of histogram
    struct {
        size_t n;
        size_t count;
    } *sorted = calloc(hist->len, sizeof(*sorted));
    for (size_t i = 0, j = 0; i < hist->size; ++i) {
        for (const Item *item = &hist->item[i]; item && item->key; item = item->next) {
            sorted[j].n = strtoul(item->key, 0, 10);
            sorted[j].count = *(size_t *)item->data;
            ++j;
        }
    }
    qsort(sorted, hist->len, sizeof(*sorted), _cmp_size_t_asc);

    // print load factor and histogram
    printf("[%f] ", (double)dict->len / (double)dict->size);
    for (size_t i = 0; i < hist->len; ++i) {
        printf("%zu: %zu%s", sorted[i].n, sorted[i].count, (i < hist->len - 1 ? ", " : "\n"));
    }

    // cleanup
    dict_free(&hist, free);
    free(sorted);
}
