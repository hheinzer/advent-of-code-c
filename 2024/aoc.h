#pragma once

#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "../cdsa/arena.h"

// memory management
Arena _arena = {0};
#define malloc(size) arena_alloc(&_arena, 1, size, alignof(max_align_t), 0)
#define calloc(count, size) arena_alloc(&_arena, count, size, alignof(max_align_t), 1)
#define realloc(ptr, size) arena_realloc(&_arena, ptr, size, alignof(max_align_t))
#define free(ptr) (void)ptr
#define memdup(ptr, size) memcpy(malloc(size), ptr, size)
#define strdup(str) strcpy(malloc(strlen(str) + 1), str)
[[gnu::constructor(1)]] void _arena_create(void)
{
    _arena = arena_create(1 << 20);
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
    Array lines = array_create_full(0, sizeof(char *), 0, 0, 0);
    FILE *file = fopen(fname, "r");
    assert(file);
    char *line = 0;
    long nc = 0;
    int c;
    while ((c = fgetc(file)) != EOF) {
        if (c == '\n') {
            assert(line);
            array_append(&lines, line);
            line = 0;
            nc = 0;
            continue;
        }
        line = realloc(line, (nc + 1) * sizeof(*line));
        line[nc++] = c;
    }
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
