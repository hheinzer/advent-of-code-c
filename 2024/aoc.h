#pragma once

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "../cdsa/array.h"
#include "../cdsa/hash.h"
#include "../cdsa/heap.h"
#include "../cdsa/hmap.h"
#include "../cdsa/list.h"
#include "../cdsa/set.h"

// automatic timing
clock_t _clock_start = 0;
[[gnu::constructor]] void _constructor(void)
{
    _clock_start = clock();
}
[[gnu::destructor]] void _destructor(void)
{
    const double wtime = (clock() - _clock_start) / (double)CLOCKS_PER_SEC;
    printf("wtime = %g s %s\n", wtime, (wtime > 1 ? "(!!!)" : ""));
}

// automatic memory management
#define defer(func) [[gnu::cleanup(func)]]
#define smart defer(_smart)
void _smart(void *ptr)
{
    free(*(void **)ptr);
}

// comparison functions
int longcmp(const void *a, const void *b)
{
    const long *_a = a, *_b = b;
    return (*_a > *_b) - (*_a < *_b);
}
int doublecmp(const void *a, const void *b)
{
    const double *_a = a, *_b = b;
    return (*_a > *_b) - (*_a < *_b);
}

// read input files
Array read_lines(const char *fname)
{
    Array lines = array_create(1000, sizeof(char *), 0, 0, free);
    FILE *file = fopen(fname, "r");
    assert(file);
    size_t size = 0, capacity = 256;
    smart char *line = calloc(capacity, sizeof(*line));
    assert(line);
    int c;
    while ((c = fgetc(file)) != EOF) {
        if (c == '\n') {
            array_append(&lines, line);
            size = 0, capacity = 256;
            line = calloc(capacity, sizeof(*line));
            continue;
        }
        if (size + 2 > capacity) {
            capacity *= 2;
            line = realloc(line, capacity * sizeof(*line));
            assert(line);
        }
        line[size++] = c;
    }
    fclose(file);
    return lines;
}

// default create functions
#define array_create_default(capacity, data_size, data_cmp) \
    array_create(capacity, data_size, data_cmp, memcpy, free)
#define list_create_default(data_size, data_cmp) list_create(data_size, data_cmp, memcpy, free)
#define hmap_create_default(capacity, data_size) \
    hmap_create(capacity, data_size, 0.75, strhash_fnv1a, memcpy, free)
#define set_create_default(capacity, data_size) \
    set_create(capacity, data_size, 0.75, memhash_fnv1a, memcpy, free)
#define heap_create_default(capacity, data_size) heap_create(capacity, data_size, memcpy, free)

// key creation on the fly
char *keyprint(const char *restrict format, ...)
{
    static char buffer[1024];
    if (!format) return buffer;
    va_list args;
    va_start(args, format);
    vsprintf(buffer, format, args);
    va_end(args);
    return buffer;
}
