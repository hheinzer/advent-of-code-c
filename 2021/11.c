/*
 * Day 11: Dumbo Octopus
 * (https://adventofcode.com/2021/day/11)
 *
 * Part 1:
 * - nothing fancy, follow the rules for flashing, keep track of who has flashed already,
 *   update the flash count
 *
 * Part 2:
 * - check if all have flashed, return step count if true
 */
#include "aoc.h"

#define N 10

size_t simulate(int grid[N + 2][N + 2], size_t n_steps)
{
    size_t count = 0;
    for (size_t step = 0; step < n_steps; ++step) {
        // increase energy level by 1
        for (size_t i = 1; i <= N; ++i) {
            for (size_t j = 1; j <= N; ++j) {
                ++grid[i][j];
            }
        }

        // flash
        int has_flashed[N + 2][N + 2] = { 0 };
        int any_has_flashed;
        do {
            any_has_flashed = 0;
            for (size_t i = 1; i <= N; ++i) {
                for (size_t j = 1; j <= N; ++j) {
                    if (!has_flashed[i][j] && grid[i][j] > 9) {
                        has_flashed[i][j] = 1;
                        ++count;
                        for (size_t ii = i - 1; ii <= i + 1; ++ii) {
                            for (size_t jj = j - 1; jj <= j + 1; ++jj) {
                                if (!has_flashed[ii][jj]) {
                                    ++grid[ii][jj];
                                }
                            }
                        }
                        grid[i][j] = 0;
                        any_has_flashed = 1;
                    }
                }
            }
        } while (any_has_flashed);

        // check if all have flashed
        int all_have_flashed = 1;
        for (size_t i = 1; i <= N; ++i) {
            for (size_t j = 1; j <= N; ++j) {
                all_have_flashed &= has_flashed[i][j];
            }
        }
        if (all_have_flashed) {
            return step;
        }
    }
    return count;
}

int main(void)
{
    // read input
    const char **line = 0;
    const size_t n_lines = lines_read(&line, "2021/input/11.txt");

    // create Octopus grid with padding
    int grid[N + 2][N + 2] = { 0 };
    for (size_t i = 0; i < N; ++i) {
        for (size_t j = 0; j < N; ++j) {
            grid[i + 1][j + 1] = line[i][j] - '0';
        }
    }

    // part 1
    printf("%zu\n", simulate(grid, 100));

    // part 2
    printf("%zu\n", 100 + simulate(grid, 1000) + 1);

    // cleanup
    lines_free(line, n_lines);
}
