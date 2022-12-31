/*
 * hi
 */

#include "aoc.c"

int main(void)
{
    char **line = 0;
    size_t n_lines = 0;
    lines_read("01.txt", &line, &n_lines);

    for (size_t i = 0; i < n_lines; ++i) {
        printf("'%s'\n", line[i]);
    }

    lines_free(line, n_lines);
}
