#pragma once

#include <assert.h>
#include <stdio.h>
#include <time.h>

#include "arena.h"
#include "list.h"

// memory management
#define alloc(a, n, t) arena_alloc(a, n, sizeof(t), alignof(typeof(t)), 0)

// automatic timing
clock_t _clock_start = 0;
[[gnu::constructor]] static void _timer_start(void) {
    _clock_start = clock();
}
[[gnu::destructor]] static void _timer_stop(void) {
    double wtime = (clock() - _clock_start) / (double)CLOCKS_PER_SEC;
    printf("wtime = %g s %s\n", wtime, (wtime > 1 ? "(!!!)" : ""));
}

// comparison functions
static int longcmp(const void *a, const void *b) {
    const long *_a = a, *_b = b;
    return (*_a > *_b) - (*_a < *_b);
}
static int doublecmp(const void *a, const void *b) {
    const double *_a = a, *_b = b;
    return (*_a > *_b) - (*_a < *_b);
}
