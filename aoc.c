/*
 * helper functions for advent of code puzzles
 */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// size of a static array
#define LEN(a) (sizeof(a) / sizeof(*a))

// type generic min and max functions
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

// cast a flat array to a multidimensional one
#define TENSOR(t, a) (__typeof__(t))a

// swap two variables
#define SWAP(a, b)              \
    do {                        \
        __typeof__(a) swap = a; \
        a = b;                  \
        b = swap;               \
    } while (0)

// type generic array summation function
#define SUM(a, n)                        \
    ({                                   \
        __typeof__(*a) s = 0;            \
        for (size_t i = 0; i < n; ++i) { \
            s += a[i];                   \
        }                                \
        s;                               \
    })

// type generic array multiplication function
#define PROD(a, n)                       \
    ({                                   \
        __typeof__(*a) p = 1;            \
        for (size_t i = 0; i < n; ++i) { \
            p *= a[i];                   \
        }                                \
        p;                               \
    })

// simple comparison function for ascending and descending order
#define CMP(T)                                                                    \
    int cmp_##T##_asc(const void *a, const void *b) { return *(T *)a - *(T *)b; } \
    int cmp_##T##_dsc(const void *a, const void *b) { return *(T *)b - *(T *)a; }
CMP(int);
CMP(long);
CMP(double);

// read all lines in file "fname" into lines, replace '\n' with '\0'
size_t lines_read(char ***line, const char *fname)
{
    // open file
    FILE *file = fopen(fname, "r");
    assert(file && "Could not open file.");

    // read file line by line
    size_t n_lines = 0;
    char *l = 0;
    size_t nc = 0;
    char c = 0;
    while ((c = fgetc(file)) != EOF) {
        if (c != '\n') {
            // append character
            l = realloc(l, ++nc * sizeof(*l));
            l[nc - 1] = c;
        } else {
            // append end of string
            l = realloc(l, ++nc * sizeof(*l));
            l[nc - 1] = 0;

            // append line
            *line = realloc(*line, ++n_lines * sizeof(**line));
            (*line)[n_lines - 1] = l;

            // initialize line
            l = calloc(0, sizeof(*l));
            nc = 0;
        }
    }
    free(l);

    // close file
    fclose(file);

    return n_lines;
}

// free all lines
void lines_free(char **line, size_t n_lines)
{
    for (size_t i = 0; i < n_lines; ++i) {
        free(line[i]);
    }
    free(line);
}
