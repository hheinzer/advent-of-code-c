/*
 * --- Day 1: Calorie Counting ---
 *
 *  Part 1:
 *  -
 */

#include "aoc.c"

int main(void)
{
    // read input
    char **line = 0;
    size_t n_lines = 0;
    lines_read("01.txt", &line, &n_lines);

    // cleanup
    lines_free(line, n_lines);
}
