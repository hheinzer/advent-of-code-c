#pragma once

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <time.h>

#include "../cdsa/arena.h"
#include "../cdsa/list.h"

// memory management
#define calloc(a, p, n) arena_alloc(a, n, sizeof(*(p)), alignof(typeof(*(p))), 0)
#define realloc(a, p, n) arena_realloc(a, p, n, sizeof(*(p)), alignof(typeof(*(p))))

// automatic timing
clock_t _clock_start = 0;
[[gnu::constructor]] void _timer_start(void) {
    _clock_start = clock();
}
[[gnu::destructor]] void _timer_stop(void) {
    double wtime = (clock() - _clock_start) / (double)CLOCKS_PER_SEC;
    printf("wtime = %g s %s\n", wtime, (wtime > 1 ? "(!!!)" : ""));
}

// comparison functions
int longcmp(const void *a, const void *b) {
    const long *_a = a, *_b = b;
    return (*_a > *_b) - (*_a < *_b);
}
