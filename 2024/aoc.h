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

// memory allocation
void *_malloc(size_t size)
{
    if (!size) return 0;
    void *ptr = malloc(size);
    assert(ptr);
    return ptr;
}
#define malloc _malloc
void *_calloc(size_t count, size_t size)
{
    if (!count || !size) return 0;
    void *ptr = calloc(count, size);
    assert(ptr);
    return ptr;
}
#define calloc _calloc
void *_realloc(void *ptr, size_t size)
{
    if (!size) {
        free(ptr);
        return 0;
    }
    ptr = realloc(ptr, size);
    assert(ptr);
    return ptr;
}
#define realloc _realloc

// memory deallocation
#define defer(func) [[gnu::cleanup(func)]]
void _smart(void *ptr)
{
    free(*(void **)ptr);
}
#define smart defer(_smart)

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
    Array lines = array_create_full(1000, sizeof(char *), 0, 0, free);
    FILE *file = fopen(fname, "r");
    assert(file);
    size_t size = 0, capacity = 256;
    char *line = calloc(capacity, sizeof(*line));
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
        }
        line[size++] = c;
    }
    free(line);
    fclose(file);
    return lines;
}

// key creation
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
