#include "dict.h"

static void *memdup(const void *src, size_t size_bytes)
{
    void *dest = malloc(size_bytes);
    if (dest) {
        memcpy(dest, src, size_bytes);
    }
    return dest;
}

static Item *item_alloc(const char *key, size_t key_size, void *data)
{
    const Item item = {
        .key = memdup(key, key_size + 1),
        .key_size = key_size,
        .data = data,
    };
    return memdup(&item, sizeof(item));
}

static void item_clear(Item *item, void (*data_free)(void *))
{
    free((void *)item->key);
    item->key = 0;
    item->key_size = 0;
    if (data_free) {
        data_free(item->data);
        item->data = 0;
    }
    item->next = 0;
}

static void item_free(Item *item, void (*data_free)(void *))
{
    item_clear(item, data_free);
    free(item);
}

Dict *dict_alloc(size_t data_size, size_t size)
{
    const Dict dict = {
        .data_size = data_size,
        .size = size,
        .item = calloc(size, sizeof(Item)),
    };
    return memdup(&dict, sizeof(dict));
}

Dict *dict_copy(const Dict *other, void *(*data_copy)(void *, const void *, size_t))
{
    Dict *dict = dict_alloc(other->data_size, other->size);
    for (size_t i = 0; i < other->size; ++i) {
        for (const Item *item = &other->item[i]; item && item->key; item = item->next) {
            void *copy = 0;
            if (data_copy) {
                copy = data_copy(malloc(dict->data_size), item->data, dict->data_size);
            } else {
                copy = item->data;
            }
            dict_insert(dict, item->key, copy);
        }
    }
    return dict;
}

void dict_free(Dict **dict, void (*data_free)(void *))
{
    for (size_t i = 0; i < (*dict)->size; ++i) {
        Item *item = &(*dict)->item[i];
        if (item->key) {
            Item *next = item->next;
            item_clear(item, data_free);
            item = next;
            while (item) {
                next = item->next;
                item_free(item, data_free);
                item = next;
            }
        }
    }
    free((*dict)->item);
    free(*dict);
    *dict = 0;
}

static size_t hash(const char *str)
{
    // djb2 (http://www.cse.yorku.ca/~oz/hash.html)
    size_t hash = 5381;
    int c;
    while ((c = *(str++))) {
        hash = ((hash << 5) + hash) + c; // hash * 33 + c
    }
    return hash;
}

int keycmp(const char *key1, size_t key_size1, const char *key2, size_t key_size2)
{
    if (key_size1 != key_size2) {
        return 1;
    }
    while (*key1 && (*key1 == *key2)) {
        ++key1;
        ++key2;
    }
    return !(*key1 == *key2);
}

void *dict_insert(Dict *dict, const char *key, void *data)
{
    const size_t key_size = strlen(key);
    const size_t i = hash(key) % dict->size;
    Item *item = &dict->item[i];
    Item *prev = item;
    while (item && item->key && keycmp(item->key, item->key_size, key, key_size)) {
        prev = item;
        item = item->next;
    }
    if (!item) { // collision: append item
        item = item_alloc(key, key_size, data);
        prev->next = item;
        ++dict->len;
        return 0;

    } else if (!item->key) { // empty spot: insert item
        memcpy(item,
            &(Item) {
                .key = memdup(key, key_size + 1),
                .key_size = key_size,
                .data = data,
            },
            sizeof(*item));
        ++dict->len;
        return 0;

    } else { // same key: update data
        void *item_data = item->data;
        item->data = data;
        return item_data;
    }
}

void *dict_remove(Dict *dict, const char *key)
{
    const size_t key_size = strlen(key);
    const size_t i = hash(key) % dict->size;
    Item *item = &dict->item[i];
    Item *prev = 0;
    while (item && item->key && keycmp(item->key, item->key_size, key, key_size)) {
        prev = item;
        item = item->next;
    }
    if (!item || !item->key) { // item not in dict
        return 0;

    } else { // delete item, return data pointer
        void *data = item->data;
        Item *next = item->next;
        if (prev) { // chained item: free and update chain
            item_free(item, 0);
            prev->next = next;

        } else { // head item: clear and move chain up
            item_clear(item, 0);
            if (next) {
                *item = *next;
                free(next);
            }
        }
        --dict->len;
        return data;
    }
}

Item *dict_find(const Dict *dict, const char *key)
{
    const size_t key_size = strlen(key);
    const size_t i = hash(key) % dict->size;
    Item *item = &dict->item[i];
    while (item && item->key && keycmp(item->key, item->key_size, key, key_size)) {
        item = item->next;
    }
    return (!item || !item->key ? 0 : item);
}
