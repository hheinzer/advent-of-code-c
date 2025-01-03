#pragma once

#include <assert.h>
#include <ctype.h>
#include <limits.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tgmath.h>

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

// cast a flat array to a multidimensional one
#define TENSOR(t, a) (__typeof__(t))a

// simple swap macro
#define SWAP(a, b)              \
    do {                        \
        __typeof__(a) swap = a; \
        a = b;                  \
        b = swap;               \
    } while (0)

// simple key creation macro
#define KEY(k, ...) (snprintf(k, sizeof(k), __VA_ARGS__), k)

// define simple min and max functions
#define MIN(a, b) ((__typeof__(a))fmin((double)(a), (double)(b)))
#define MAX(a, b) ((__typeof__(a))fmax((double)(a), (double)(b)))

// define simple comparison functions for ascending and descending order
#define CMP(T)                                            \
    int cmp_##T##_asc(const void *a, const void *b) {     \
        return (*(T *)a > *(T *)b) - (*(T *)a < *(T *)b); \
    }                                                     \
    int cmp_##T##_dsc(const void *a, const void *b) {     \
        return (*(T *)a < *(T *)b) - (*(T *)a > *(T *)b); \
    }

// read all lines in file "fname" into lines, replace '\n' with '\0'
size_t lines_read(const char ***line, const char *fname) {
    // open file
    FILE *file = fopen(fname, "r");
    assert(file && "Could not open file.");

    // read file line by line
    size_t n_lines = 0;
    char *l = 0;
    size_t nc = 0;
    int c = 0;
    while ((c = fgetc(file)) != EOF) {
        if (c != '\n') {
            // append character
            l = realloc(l, ++nc * sizeof(*l));
            l[nc - 1] = (char)c;
        } else {
            // append end of string
            l = realloc(l, ++nc * sizeof(*l));
            l[nc - 1] = 0;

            // append line
            *line = realloc(*line, ++n_lines * sizeof(**line));
            (*line)[n_lines - 1] = l;

            // initialize line
            l = 0;
            nc = 0;
        }
    }
    free(l);

    // close file
    fclose(file);

    assert(n_lines > 0);
    return n_lines;
}

// free all lines
void lines_free(const char **line, size_t n_lines) {
    for (size_t i = 0; i < n_lines; ++i) {
        free((void *)line[i]);
    }
    free(line);
}

// find index of first line that matches
size_t line_find(const char **line, size_t n_lines, const char *find) {
    for (size_t i = 0; i < n_lines; ++i) {
        if (!strcmp(line[i], find)) {
            return i;
        }
    }
    assert(0 && "No matching line found.");
}
