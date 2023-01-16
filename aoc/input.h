#ifndef INPUT_H
#define INPUT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// line by line content of input file
typedef struct Input {
    size_t n_lines; // number of lines
    const char **line; // pointer to line
} Input;

// allocate input, read all lines from input file
// fname: file name of input file
// return: pointer to input
Input *input_alloc(const char *fname);

// free input
// self: pointer to input
void input_free(Input *self);

// find matching line in input
// self: pointer to input
// i0: starting index for search
// find: string that is searched for
// return: index of matching line, 0 if no match, or i0 out of range
size_t input_find(const Input *self, size_t i0, const char *find);

#endif
