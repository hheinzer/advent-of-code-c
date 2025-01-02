#pragma once

#include <assert.h>
#include <ctype.h>
#include <omp.h>
#include <stdio.h>
#include <time.h>

#include "../cdsa/arena.h"
#include "../cdsa/dict.h"
#include "../cdsa/list.h"
#include "../cdsa/set.h"

// memory management
#define calloc(a, p, n) arena_alloc(a, n, sizeof(*(p)), alignof(typeof(*(p))), 0)
#define realloc(a, p, n) arena_realloc(a, p, n, sizeof(*(p)), alignof(typeof(*(p))))

// automatic timing
clock_t _wtime_start = 0;
[[gnu::constructor]] void _timer_start(void) {
    _wtime_start = omp_get_wtime();
}
[[gnu::destructor]] void _timer_stop(void) {
    double wtime = omp_get_wtime() - _wtime_start;
    printf("wtime = %g s %s\n", wtime, (wtime > 1 ? "(!!!)" : ""));
}

// comparison functions
int longcmp(const void *a, const void *b, void *) {
    const long *_a = a, *_b = b;
    return (*_a > *_b) - (*_a < *_b);
}

// grid functions
typedef struct {
    long rows;
    long cols;
    char *data;
} Grid;
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
    char old = grid_get(grid, r, c);
    if (old) {
        grid->data[r * grid->cols + c] = new;
    }
    return old;
}

// integer math functions
long lpow(long a, long b) {
    long c = 1;
    for (long i = 0; i < b; i++) {
        c *= a;
    }
    return c;
}
