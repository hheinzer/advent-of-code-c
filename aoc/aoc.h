/*
 * helper functions for advent of code puzzles
 */
#ifndef AOC_H
#define AOC_H

#include <assert.h>
#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "htable.h"
#include "list.h"

// size of a static array
#define LEN(a) (sizeof(a) / sizeof(*a))

// type generic min and max functions
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

// sign of an integer
#define SIGN(a) (((a) > 0) - ((a) < 0))

// integer ceil division, positive integers only
#define CEIL(a, b) (1 + (((a)-1) / (b)))

// cast a flat array to a multidimensional one
#define TENSOR(t, a) (__typeof__(t))a

// simple swap macro
#define SWAP(a, b)              \
    do {                        \
        __typeof__(a) swap = a; \
        a = b;                  \
        b = swap;               \
    } while (0)

// create a copy of a on the heap
#define COPY(a) memcpy(malloc(sizeof(a)), &a, sizeof(a))

// define simple comparison functions for ascending and descending order
#define CMP(T)                                                                    \
    int cmp_##T##_asc(const void *a, const void *b) { return *(T *)a - *(T *)b; } \
    int cmp_##T##_dsc(const void *a, const void *b) { return *(T *)b - *(T *)a; }

// read all lines in file "fname" into lines, replace '\n' with '\0'
size_t lines_read(const char ***line, const char *fname);

// free all lines
void lines_free(const char **line, size_t n_lines);

// find index of first line that matches
size_t line_find(const char **line, size_t n_lines, const char *find);

#endif