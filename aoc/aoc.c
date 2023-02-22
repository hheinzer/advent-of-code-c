#include "aoc.h"

static double _timer_start = 0;
__attribute__((constructor)) static void run_before_main(void)
{
    _timer_start = clock();
}

__attribute__((destructor)) static void run_after_main(void)
{
    const double wtime = (clock() - _timer_start) / CLOCKS_PER_SEC;
    if (wtime > 1.0) {
        printf("wtime = %g s (!!!)\n", wtime);
    } else {
        printf("wtime = %g s\n", wtime);
    }
}

size_t lines_read(const char ***line, const char *fname)
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

void lines_free(const char **line, size_t n_lines)
{
    for (size_t i = 0; i < n_lines; ++i) {
        free((void *)line[i]);
    }
    free(line);
}

size_t line_find(const char **line, size_t n_lines, const char *find)
{
    for (size_t i = 0; i < n_lines; ++i) {
        if (!strcmp(line[i], find)) {
            return i;
        }
    }
    assert(!"No matching line found.");
}
