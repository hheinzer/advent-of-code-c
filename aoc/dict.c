#include "dict.h"

static Item item_create(const char *key, void *data)
{
    const Item item = {
        .key = strcpy(malloc(strlen(key) + 1), key),
        .data = data,
    };
    return item;
}

static Item *item_alloc(const char *key, void *data)
{
    const Item item = item_create(key, data);
    return memcpy(malloc(sizeof(item)), &item, sizeof(item));
}

static void item_clear(Item *item, void (*data_free)())
{
    free(item->key);
    item->key = 0;
    if (data_free) {
        data_free(item->data);
        item->data = 0;
    }
    item->next = 0;
}

static void item_free(Item *item, void (*data_free)())
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
    return memcpy(malloc(sizeof(dict)), &dict, sizeof(dict));
}

void dict_free(Dict **dict, void (*data_free)())
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

void *dict_insert(Dict *dict, const char *key, void *data)
{
    const size_t i = hash(key) % dict->size;
    Item *item = &dict->item[i];
    Item *prev = 0;
    while (item && item->key && strcmp(item->key, key)) {
        prev = item;
        item = item->next;
    }
    if (!item) { // collision: append item
        item = item_alloc(key, data);
        prev->next = item;
        ++dict->len;
        return 0;

    } else if (!item->key) { // empty spot: insert item
        *item = item_create(key, data);
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
    const size_t i = hash(key) % dict->size;
    Item *item = &dict->item[i];
    Item *prev = 0;
    while (item && item->key && strcmp(item->key, key)) {
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

Item *dict_find(Dict *dict, const char *key)
{
    const size_t i = hash(key) % dict->size;
    Item *item = &dict->item[i];
    while (item && item->key && strcmp(item->key, key)) {
        item = item->next;
    }
    return (!item || !item->key ? 0 : item);
}
