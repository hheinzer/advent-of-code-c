/*
 * Day 25: Sea Cucumber
 * (https://adventofcode.com/2021/day/25)
 *
 * Part 1:
 * - create a map of the cucumbers and a copy
 * - update positions until cucumbers get stuck
 * - print number of iterations
 */
#include "aoc.h"

int main(void) {
    // read input
    const char **line = 0;
    const size_t n_lines = lines_read(&line, "2021/input/25.txt");

    // read map
    const size_t ni = n_lines;
    const size_t nj = strlen(line[0]);
    char(*map)[nj] = calloc(ni, sizeof(*map));
    for (size_t i = 0; i < ni; ++i) {
        for (size_t j = 0; j < nj; ++j) {
            map[i][j] = line[i][j];
        }
    }

    // create copy of map
    char(*copy)[nj] = memdup(map, ni * sizeof(*map));

    // move cucumbers until they get stuck
    size_t count = 0;
    while (1) {
        int is_struck = 1;
        // move east
        for (size_t i = 0; i < ni; ++i) {
            for (size_t j = 0; j < nj; ++j) {
                if ((map[i][j] == '>') && (map[i][(j + 1) % nj] == '.')) {
                    copy[i][j] = '.';
                    copy[i][(j + 1) % nj] = '>';
                    is_struck = 0;
                }
            }
        }
        memcpy(map, copy, ni * sizeof(*map));
        // move south
        for (size_t i = 0; i < ni; ++i) {
            for (size_t j = 0; j < nj; ++j) {
                if ((map[i][j] == 'v') && (map[(i + 1) % ni][j] == '.')) {
                    copy[i][j] = '.';
                    copy[(i + 1) % ni][j] = 'v';
                    is_struck = 0;
                }
            }
        }
        memcpy(map, copy, ni * sizeof(*map));
        // increment count
        ++count;
        // check if cucumbers are stuck
        if (is_struck) break;
    }

    // part 1
    printf("%zu\n", count);

    // cleanup
    lines_free(line, n_lines);
    free(map);
    free(copy);
}
