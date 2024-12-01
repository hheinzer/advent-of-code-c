/*
 * Day 1: Sonar Sweep
 * (https://adventofcode.com/2021/day/1)
 *
 * Part 1:
 * - go through list and compare measurements
 *
 * Part 2:
 * - sum up last three values and compare
 */
#include "aoc.h"

int main(void)
{
    // read input
    const char **line = 0;
    const size_t n_lines = lines_read(&line, "2021/input/01.txt");

    // count the number of increases
    size_t count1 = 0;
    for (size_t i = 1; i < n_lines; ++i) {
        const long a = strtol(line[i - 1], 0, 10);
        const long b = strtol(line[i], 0, 10);
        if (b > a) {
            ++count1;
        }
    }

    // part 1
    printf("%zu\n", count1);

    // count the number of increases
    size_t count2 = 0;
    for (size_t i = 3; i < n_lines; ++i) {
        long suma = 0;
        long sumb = 0;
        for (size_t j = 0; j < 3; ++j) {
            suma += strtol(line[i - 3 + j], 0, 10);
            sumb += strtol(line[i - 2 + j], 0, 10);
        }
        if (sumb > suma) {
            ++count2;
        }
    }

    // part 2
    printf("%zu\n", count2);

    // cleanup
    lines_free(line, n_lines);
}
