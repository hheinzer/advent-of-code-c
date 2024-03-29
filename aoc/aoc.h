/*
 * helper functions for advent of code puzzles
 */
#ifndef AOC_H
#define AOC_H

#include <assert.h>
#include <ctype.h>
#include <limits.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tgmath.h>
#include <time.h>

#include "dict.h"
#include "heap.h"
#include "list.h"
#include "memory.h"
#include "queue.h"
#include "stack.h"

// size of a static array
#define LEN(a) (sizeof(a) / sizeof(*a))

// sign of an integer
#define SIGN(a) (((a) > 0) - ((a) < 0))

// integer ceil division, positive integers only
#define CEIL(a, b) ((__typeof__(a))ceil((double)(a) / (double)(b)))

// cast a flat array to a multidimensional one
#define TENSOR(t, a) (__typeof__(t))a

// simple swap macro
#define SWAP(a, b)                                                                      \
    do {                                                                                \
        __typeof__(a) swap = a;                                                         \
        a = b;                                                                          \
        b = swap;                                                                       \
    } while (0)

// simple key creation macro
#define KEY(k, ...) (snprintf(k, sizeof(k), __VA_ARGS__), k)

// define simple min and max functions
#define MIN(a, b) ((__typeof__(a))fmin((double)(a), (double)(b)))
#define MAX(a, b) ((__typeof__(a))fmax((double)(a), (double)(b)))

// define simple comparison functions for ascending and descending order
#define CMP(T)                                                                          \
    int cmp_##T##_asc(const void *a, const void *b)                                     \
    {                                                                                   \
        return (*(T *)a > *(T *)b) - (*(T *)a < *(T *)b);                               \
    }                                                                                   \
    int cmp_##T##_dsc(const void *a, const void *b)                                     \
    {                                                                                   \
        return (*(T *)a < *(T *)b) - (*(T *)a > *(T *)b);                               \
    }

// read all lines in file "fname" into lines, replace '\n' with '\0'
size_t lines_read(const char ***line, const char *fname);

// free all lines
void lines_free(const char **line, size_t n_lines);

// find index of first line that matches
size_t line_find(const char **line, size_t n_lines, const char *find);

#endif
