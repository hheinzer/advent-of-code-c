#pragma once

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "../cdsa/arena.h"

// memory management
Arena _arena = {0};
#define malloc(size) arena_malloc(&_arena, size)
#define calloc(count, size) arena_calloc(&_arena, count, size)
#define realloc(ptr, size) arena_realloc(&_arena, ptr, size)
#define free(ptr) arena_free(&_arena, ptr)
#define memdup(ptr, size) memcpy(arena_malloc(&_arena, size), ptr, size)
#define strdup(str) strcpy(arena_malloc(&_arena, strlen(str) + 1), str)
[[gnu::constructor(1)]] void _arena_create(void)
{
    _arena = arena_create(MB, alignof(size_t));
}
[[gnu::destructor(1)]] void _arena_clear(void)
{
    arena_clear(&_arena);
}

#include "../cdsa/array.h"
#include "../cdsa/hash.h"
#include "../cdsa/heap.h"
#include "../cdsa/hmap.h"
#include "../cdsa/list.h"
#include "../cdsa/set.h"

// automatic timing
clock_t _clock_start = 0;
[[gnu::constructor(2)]] void _timer_start(void)
{
    _clock_start = clock();
}
[[gnu::destructor(2)]] void _timer_stop(void)
{
    const double wtime = (clock() - _clock_start) / (double)CLOCKS_PER_SEC;
    printf("wtime = %g s %s\n", wtime, (wtime > 1 ? "(!!!)" : ""));
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
    Array lines = array_create_full(1000, sizeof(char *), 0, 0, 0);
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
