/*
 * helper functions for advent of code puzzles
 */
#include <assert.h>
#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// size of a static array
#define LEN(a) (sizeof(a) / sizeof(*a))

// type generic min and max functions
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

// sign of an integer
#define SIGN(a) (((a) > 0) - ((a) < 0))

// integer ceil division, positive integers only
#define CEIL(a, b) (1 + (((a)-1) / (b)))

// cast a flat array to a multidimensional one
#define TENSOR(t, a) (__typeof__(t))a

// simple swap macro
#define SWAP(a, b)              \
    do {                        \
        __typeof__(a) swap = a; \
        a = b;                  \
        b = swap;               \
    } while (0)

// define simple comparison functions for ascending and descending order
#define CMP(T)                                                                    \
    int cmp_##T##_asc(const void *a, const void *b) { return *(T *)a - *(T *)b; } \
    int cmp_##T##_dsc(const void *a, const void *b) { return *(T *)b - *(T *)a; }

// functions that before and after main, used for timing
static double _timer_start = 0;
__attribute__((constructor)) void run_before_main()
{
    _timer_start = clock();
}
__attribute__((destructor)) void run_after_main()
{
    printf("wtime = %g s\n", (clock() - _timer_start) / CLOCKS_PER_SEC);
}

// read all lines in file "fname" into lines, replace '\n' with '\0'
size_t lines_read(const char ***line, const char *fname)
{
    // open file
    FILE *file = fopen(fname, "r");
    assert(file && "Could not open file.");

    // read file line by line
    size_t n_lines = 0;
    char *l = 0;
    size_t nc = 0;
    char c = 0;
    while ((c = fgetc(file)) != EOF) {
        if (c != '\n') {
            // append character
            l = realloc(l, ++nc * sizeof(*l));
            l[nc - 1] = c;
        } else {
            // append end of string
            l = realloc(l, ++nc * sizeof(*l));
            l[nc - 1] = 0;

            // append line
            *line = realloc(*line, ++n_lines * sizeof(**line));
            (*line)[n_lines - 1] = l;

            // initialize line
            l = calloc(0, sizeof(*l));
            nc = 0;
        }
    }
    free(l);

    // close file
    fclose(file);

    return n_lines;
}

// free all lines
void lines_free(const char **line, size_t n_lines)
{
    for (size_t i = 0; i < n_lines; ++i) {
        free((void *)line[i]);
    }
    free(line);
}

// find index of first line that matches
size_t line_index(const char **line, size_t n_lines, const char *find)
{
    for (size_t i = 0; i < n_lines; ++i) {
        if (!strcmp(line[i], find)) {
            return i;
        }
    }
    assert(!"No matching line found.");
}

// general purpose (doubly linked) list data structure
typedef struct List List;
typedef struct ListElement ListElement;
struct List {
    const size_t data_size;
    size_t nelem;
    ListElement *head;
    ListElement *tail;
};
struct ListElement {
    void *data;
    ListElement *next;
    ListElement *prev;
};

// create list
List *list_create(size_t data_size)
{
    List *list = malloc(sizeof(*list));
    memcpy(list, &(List) { .data_size = data_size }, sizeof(*list));
    return list;
}

// free list
void list_free(List *list)
{
    for (ListElement *elem = list->head, *next; elem; elem = next) {
        next = elem->next;
        free(elem->data);
        free(elem);
    }
    free(list);
}

// insert element into list at position ielem, no copying of data
void list_insert(List *list, size_t ielem, void *data)
{
    assert(ielem <= list->nelem && "Illegal index encountered.");

    // create new element
    ListElement *elem = calloc(1, sizeof(*elem));
    elem->data = data;

    if (list->nelem == 0) {
        // empty list: new element is head and tail of list
        list->head = elem;
        list->tail = elem;

    } else if (ielem == 0) {
        // new element is new head of list
        elem->next = list->head;
        list->head->prev = elem;
        list->head = elem;

    } else if (ielem == list->nelem) {
        // new element is new tail of list
        elem->prev = list->tail;
        list->tail->next = elem;
        list->tail = elem;

    } else {
        // insert element into list, take shortest path
        ListElement *next = 0;
        if (ielem <= list->nelem - 1 - ielem) {
            next = list->head;
            for (size_t i = 0; i < ielem; ++i) {
                next = next->next;
            }

        } else {
            next = list->tail;
            for (size_t i = 0; i < list->nelem - 1 - ielem; ++i) {
                next = next->prev;
            }
        }
        elem->next = next;
        elem->prev = next->prev;
        elem->next->prev = elem;
        elem->prev->next = elem;
    }

    // increase element counter
    ++list->nelem;
}
void list_push_front(List *list, void *data)
{
    list_insert(list, 0, data);
}
void list_push_back(List *list, void *data)
{
    list_insert(list, list->nelem, data);
}

// insert element into list at position ielem, copy data
void list_insert_copy(List *list, size_t ielem, const void *data, void *(*data_copy)(void *, const void *, size_t))
{
    void *copy = malloc(list->data_size);
    data_copy(copy, data, list->data_size);
    list_insert(list, ielem, copy);
}
void list_push_front_copy(List *list, const void *data, void *(*data_copy)(void *, const void *, size_t))
{
    void *copy = malloc(list->data_size);
    data_copy(copy, data, list->data_size);
    list_push_front(list, copy);
}
void list_push_back_copy(List *list, const void *data, void *(*data_copy)(void *, const void *, size_t))
{
    void *copy = malloc(list->data_size);
    data_copy(copy, data, list->data_size);
    list_push_back(list, copy);
}

// remove element from list at position ielem, return its data pointer
void *list_remove(List *list, size_t ielem)
{
    assert(list->nelem > 0 && "Cannot pop element from empty list.");
    assert(ielem < list->nelem && "Illegal index encountered.");

    ListElement *elem = 0;
    if (list->nelem == 1) {
        // element is last element in list
        elem = list->head;
        list->head = 0;
        list->tail = 0;

    } else if (ielem == 0) {
        // element is head of list
        elem = list->head;
        list->head = list->head->next;
        list->head->prev = 0;

    } else if (ielem == list->nelem - 1) {
        // element is tail of list
        elem = list->tail;
        list->tail = list->tail->prev;
        list->tail->next = 0;

    } else {
        // find element in list, take shortest path
        if (ielem <= list->nelem - 1 - ielem) {
            elem = list->head;
            for (size_t i = 0; i < ielem; ++i) {
                elem = elem->next;
            }

        } else {
            elem = list->tail;
            for (size_t i = 0; i < list->nelem - 1 - ielem; ++i) {
                elem = elem->prev;
            }
        }
        elem->next->prev = elem->prev;
        elem->prev->next = elem->next;
    }

    // get data pointer
    void *data = elem->data;

    // remove element
    free(elem);

    // decrease element counter
    --list->nelem;

    return data;
}
void *list_pop_front(List *list)
{
    return list_remove(list, 0);
}
void *list_pop_back(List *list)
{
    return list_remove(list, list->nelem - 1);
}

// get data pointer of element at ielem, return 0 it doesn't exist
void *list_get(List *list, size_t ielem)
{
    if (ielem >= list->nelem) {
        return 0;
    }

    // find element in list, take shortest path
    ListElement *elem = 0;
    if (ielem <= list->nelem - 1 - ielem) {
        elem = list->head;
        for (size_t i = 0; i < ielem; ++i) {
            elem = elem->next;
        }

    } else {
        elem = list->tail;
        for (size_t i = 0; i < list->nelem - 1 - ielem; ++i) {
            elem = elem->prev;
        }
    }

    return elem->data;
}

// sort list elements
void list_sort(List *list, int (*compar)(const void *, const void *))
{
    // create data array
    char *data = malloc(list->nelem * list->data_size);
    ListElement *elem = list->head;
    for (size_t i = 0; i < list->nelem; ++i, elem = elem->next) {
        memcpy(data + i * list->data_size, elem->data, list->data_size);
    }

    // sort array
    qsort(data, list->nelem, list->data_size, compar);

    // write back data array
    elem = list->head;
    for (size_t i = 0; i < list->nelem; ++i, elem = elem->next) {
        memcpy(elem->data, data + i * list->data_size, list->data_size);
    }

    // cleanup
    free(data);
}

// search for an element in the list, return index of first match
size_t list_search(List *list, const void *data, int (*compar)(const void *, const void *))
{
    ListElement *elem = list->head;
    for (size_t i = 0; i < list->nelem; ++i) {
        if (!compar(elem->data, data)) {
            return i;
        }
        elem = elem->next;
    }
    assert(!"Element not in list.");
}

// general purpose hash table
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
Htable *htable_create(size_t data_size, size_t table_size)
{
    Htable *htable = malloc(sizeof(*htable));
    memcpy(htable, &(Htable) { .data_size = data_size }, sizeof(*htable));
    htable->table_size = table_size;
    htable->elem = calloc(table_size, sizeof(*htable->elem));
    return htable;
}

// free hash table
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

// hash function
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

// insert element into hash table, no copying of data
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

// insert element into hash table, copy data
void htable_insert_copy(Htable *htable, const char *key, const void *data, void *(*data_copy)(void *, const void *, size_t))
{
    void *copy = malloc(htable->data_size);
    data_copy(copy, data, htable->data_size);
    htable_insert(htable, key, copy);
}

// remove element from hash table, return its data pointer
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

// search element in hash table, return its data pointer, return 0 if it doesn't exist
HtableElement *htable_search(const Htable *htable, const char *key)
{
    const size_t ielem = htable_hash(key) % htable->table_size;
    HtableElement *elem = htable->elem[ielem];

    while (elem && elem->key && strcmp(elem->key, key)) {
        elem = elem->next;
    }

    return elem;
}
