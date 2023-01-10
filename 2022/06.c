/*
 * Day 6: Tuning Trouble
 * (https://adventofcode.com/2022/day/6)
 *
 * Part 1:
 * - write function to check if the characters in a string are unique
 * - chack every pair of 4 chars from beginning
 *
 * Part 2:
 * - chack every pair of 14 chars from beginning
 */
#include "../aoc.h"

int all_chars_unique(const char *str, size_t n)
{
    for (size_t i = 0; i < n - 1; ++i) {
        for (size_t j = i + 1; j < n; ++j) {
            if (str[i] == str[j]) {
                return 0;
            }
        }
    }
    return 1;
}

int main(void)
{
    // read input
    const char **line = 0;
    const size_t n_lines = lines_read(&line, "06.txt");

    // part 1
    const size_t n = strlen(line[0]);
    for (size_t m = 4, i = m - 1; i < n; ++i) {
        if (all_chars_unique(&line[0][i - (m - 1)], m)) {
            printf("%zu\n", i + 1);
            break;
        }
    }

    // part 2
    for (size_t m = 14, i = m - 1; i < n; ++i) {
        if (all_chars_unique(&line[0][i - (m - 1)], m)) {
            printf("%zu\n", i + 1);
            break;
        }
    }

    // cleanup
    lines_free(line, n_lines);
}
