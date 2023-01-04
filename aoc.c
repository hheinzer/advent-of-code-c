/*
 * helper functions for advent of code puzzles
 */
#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// size of a static array
#define LEN(a) (sizeof(a) / sizeof(*a))

// type generic min and max functions
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

// cast a flat array to a multidimensional one
#define TENSOR(t, a) (__typeof__(t))a

// swap two variables
#define SWAP(a, b)              \
    do {                        \
        __typeof__(a) swap = a; \
        a = b;                  \
        b = swap;               \
    } while (0)

// define simple comparison function for ascending and descending order
#define CMP(T)                                                                    \
    int cmp_##T##_asc(const void *a, const void *b) { return *(T *)a - *(T *)b; } \
    int cmp_##T##_dsc(const void *a, const void *b) { return *(T *)b - *(T *)a; }

// read all lines in file "fname" into lines, replace '\n' with '\0'
size_t lines_read(char ***line, const char *fname)
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
void lines_free(char **line, size_t n_lines)
{
    for (size_t i = 0; i < n_lines; ++i) {
        free(line[i]);
    }
    free(line);
}

// find index of first line that matches
size_t line_index(char **line, size_t n_lines, const char *find)
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
    size_t data_size;
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
List list_create(size_t data_size)
{
    return (List) { .data_size = data_size };
}

// free list
void list_free(List *list)
{
    ListElement *elem = list->head;
    for (size_t i = 0; i < list->nelem; ++i) {
        ListElement *next = elem->next;
        free(elem->data);
        free(elem);
        elem = next;
    }
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
void list_insert_copy(List *list, size_t ielem, void *data)
{
    void *copy = malloc(list->data_size);
    memcpy(copy, data, list->data_size);
    list_insert(list, ielem, copy);
}
void list_push_front_copy(List *list, void *data)
{
    void *copy = malloc(list->data_size);
    memcpy(copy, data, list->data_size);
    list_push_front(list, copy);
}
void list_push_back_copy(List *list, void *data)
{
    void *copy = malloc(list->data_size);
    memcpy(copy, data, list->data_size);
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

// copy list
List list_copy(List *list)
{
    List copy = list_create(list->data_size);
    for (ListElement *elem = list->head; elem; elem = elem->next) {
        list_push_back_copy(&copy, elem->data);
    }
    return copy;
}
