/*
 * Day 3: Rucksack Reorganization
 * (https://adventofcode.com/2022/day/3)
 *
 * Part 1:
 * - find common chars in string halves
 * - compute priority via custom char to int conversion function
 *
 * Part 2:
 * - find common char in 3 subsequent strings
 * - add up priorities
 */
#include "aoc.h"

char find_common_char_2(const char *str1, const char *str2, size_t n)
{
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < n; ++j) {
            if (str1[i] == str2[j]) {
                return str1[i];
            }
        }
    }
    assert(!"No common character found.");
}

int item2prio(char item)
{
    if (('a' <= item) && (item <= 'z')) {
        return 1 + item - 'a';
    } else if (('A' <= item) && (item <= 'Z')) {
        return 27 + item - 'A';
    }
    assert(!"Illegal item encountered.");
}

char find_common_char_3(const char *str1, const char *str2, const char *str3)
{
    const size_t ni = strlen(str1);
    const size_t nj = strlen(str2);
    const size_t nk = strlen(str3);
    for (size_t i = 0; i < ni; ++i) {
        for (size_t j = 0; j < nj; ++j) {
            if (str1[i] == str2[j]) {
                for (size_t k = 0; k < nk; ++k) {
                    if (str1[i] == str3[k]) {
                        return str1[i];
                    }
                }
            }
        }
    }
    assert(!"No common character found.");
}

int main(void)
{
    // read input
    const char **line = 0;
    const size_t n_lines = lines_read(&line, "2022/input/03.txt");

    // find common char between first and second half of every line and add up priorities
    long sum_prio1 = 0;
    for (size_t i = 0; i < n_lines; ++i) {
        const size_t n = strlen(line[i]) / 2;
        sum_prio1 += item2prio(find_common_char_2(&line[i][0], &line[i][n], n));
    }

    // part 1
    printf("%ld\n", sum_prio1);

    // find common char of three lines at a time
    long sum_prio2 = 0;
    for (size_t i = 0; i < n_lines; i += 3) {
        sum_prio2 += item2prio(find_common_char_3(line[i + 0], line[i + 1], line[i + 2]));
    }

    // part 2
    printf("%ld\n", sum_prio2);

    // cleanup
    lines_free(line, n_lines);
}
