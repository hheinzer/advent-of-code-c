#include "input.h"

Input *input_alloc(const char *fname)
{
    FILE *file = fopen(fname, "r");
    if (!file) {
        return 0;
    }

    Input *input = malloc(sizeof(*input));
    size_t max_n_lines = 100;
    input->n_lines = 0;
    input->line = calloc(max_n_lines, sizeof(*input->line));

    size_t max_n_char = 256;
    size_t n_char = 0;
    char *line = calloc(max_n_char, sizeof(*line));
    char c;
    while ((c = fgetc(file)) != EOF) {
        switch (c) {
        case '\n':
            if (++input->n_lines == max_n_lines) {
                max_n_lines *= 2;
                input->line = realloc(input->line, max_n_lines * sizeof(*input->line));
            }
            input->line[input->n_lines - 1] = line;
            n_char = 0;
            line = calloc(max_n_char, sizeof(*line));
            break;
        default:
            if (++n_char == max_n_char) {
                max_n_char *= 2;
                line = realloc(line, max_n_char * sizeof(*line));
            }
            line[n_char - 1] = c;
        }
    }
    free(line);

    fclose(file);

    return input;
}

void input_free(Input *self)
{
    for (size_t i = 0; i < self->n_lines; ++i) {
        free((void *)self->line[i]);
    }
    free(self->line);
    free(self);
}

size_t input_find(const Input *self, size_t i0, const char *find)
{
    for (size_t i = i0; i < self->n_lines; ++i) {
        if (!strcmp(self->line[i], find)) {
            return i;
        }
    }
    return self->n_lines;
}
