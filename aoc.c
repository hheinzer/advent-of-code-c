/*
 * helper functions for advent of code puzzles
 */

#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define LEN(a) (sizeof(a) / sizeof(*a))
#define TENSOR(t, a) (__typeof__(t))a

/*
 * read all lines in file "fname" into lines, replace '\n' with '\0'
 */
static void lines_read(const char *fname, char ***lines, size_t *n_lines)
{
    // open file
    FILE *file = fopen(fname, "r");
    assert(file && "Could not open file.");

    // read file line by line
    char *line = 0;
    size_t nc = 0;
    char c = 0;
    while ((c = fgetc(file)) != EOF) {
        if (c != '\n') {
            // append character
            line = reallocarray(line, ++nc, sizeof(*line));
            line[nc - 1] = c;
        } else {
            // append end of string
            line = reallocarray(line, ++nc, sizeof(*line));
            line[nc - 1] = 0;

            // append line
            *lines = reallocarray(*lines, ++(*n_lines), sizeof(**lines));
            (*lines)[*n_lines - 1] = line;

            // initialize line
            line = calloc(0, sizeof(*line));
            nc = 0;
        }
    }
    free(line);

    // close file
    fclose(file);
}

/*
 * free all lines
 */
static void lines_free(char **line, size_t n_lines)
{
    for (size_t i = 0; i < n_lines; ++i) {
        free(line[i]);
    }
    free(line);
}
