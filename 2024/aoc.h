#pragma once

#include <assert.h>
#include <ctype.h>
#include <limits.h>
#include <math.h>
#include <stdio.h>

#include "../cdsa/arena.h"
#include "../cdsa/dict.h"
#include "../cdsa/heap.h"
#include "../cdsa/list.h"
#include "../cdsa/set.h"

// memory management
#define calloc(A, P, N) arena_alloc(A, N, sizeof(*(P)), alignof(typeof(*(P))), 0)
#define realloc(A, P, N) arena_realloc(A, P, N, sizeof(*(P)), alignof(typeof(*(P))))

// convenience functions
#define countof(A) (sizeof(A) / sizeof(*(A)))
#define array_for_each(T, I, ...) for (T A[] = {__VA_ARGS__}, *I = A; I < A + countof(A); I++)

// comparison functions
int longcmp(const void *a, const void *b, void *) {
    const long *_a = a, *_b = b;
    return (*_a > *_b) - (*_a < *_b);
}

// integer math functions
long lpow(long a, long b) {
    long c = 1;
    for (long i = 0; i < b; i++) {
        c *= a;
    }
    return c;
}
long modulo(long a, long b) {
    return ((a % b) + b) % b;
}

// parse functions
char *string_parse(const char *fname, const char *skip, Arena *arena) {
    char *input = calloc(arena, input, 1);
    FILE *file = fopen(fname, "r");
    assert(file);
    long n = 1;
    int c;
    while ((c = fgetc(file)) != EOF) {
        if (skip && strchr(skip, c)) {
            continue;
        }
        input = realloc(arena, input, ++n);
        input[n - 2] = c;
    }
    input[n - 1] = 0;
    fclose(file);
    return input;
}

// grid functions
typedef struct {
    long r;
    long c;
} Vec2;
typedef struct {
    long rows;
    long cols;
    char *data;
} Grid;
Grid grid_create(long rows, long cols, char c, Arena *arena) {
    Grid grid = {0};
    grid.rows = rows;
    grid.cols = cols;
    grid.data = calloc(arena, grid.data, grid.rows * grid.cols + 1);
    memset(grid.data, c, grid.rows * grid.cols);
    return grid;
}
Grid grid_parse(const char *fname, Arena *arena) {
    Grid grid = {0};
    grid.data = calloc(arena, grid.data, 1);
    FILE *file = fopen(fname, "r");
    assert(file);
    char line[256];
    while (fgets(line, sizeof(line), file)) {
        *strchr(line, '\n') = 0;
        grid.rows += 1;
        grid.cols = strlen(line);
        grid.data = realloc(arena, grid.data, grid.rows * grid.cols + 1);
        strncat(grid.data, line, grid.cols);
    }
    fclose(file);
    return grid;
}
char grid_get(const Grid *grid, long r, long c) {
    if (r < 0 || grid->rows <= r || c < 0 || grid->cols <= c) {
        return 0;
    }
    return grid->data[r * grid->cols + c];
}
char grid_set(Grid *grid, long r, long c, char new) {
    if (r < 0 || grid->rows <= r || c < 0 || grid->cols <= c) {
        return 0;
    }
    char old = grid->data[r * grid->cols + c];
    grid->data[r * grid->cols + c] = new;
    return old;
}
void grid_print(const Grid *grid) {
    for (long r = 0; r < grid->rows; r++) {
        for (long c = 0; c < grid->cols; c++) {
            putchar(grid_get(grid, r, c));
        }
        putchar('\n');
    }
}
void grid_write_pgm(const Grid *grid, const char *fname) {
    FILE *file = fopen(fname, "wb");
    assert(file);
    fprintf(file, "P5\n%ld %ld\n255\n", grid->cols, grid->rows);
    fwrite(grid->data, 1, grid->cols * grid->rows, file);
    fclose(file);
}
