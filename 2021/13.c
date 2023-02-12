/*
 * Day 13: Transparent Origami
 * (https://adventofcode.com/2021/day/13)
 *
 * Part 1:
 * - grid a grid of dots
 * - fold at the given values along the given axis
 *   - xy_new = 2 * value - xy_old
 * - fold once and go through count dots in grid
 *
 * Part 2:
 * - fold all instructions
 * - print result (fits into 40x6 characters)
 */
#include "aoc.h"

void paper_fold(const char **line,
    size_t ny, size_t nx, int grid[ny][nx], size_t i0, size_t n)
{
    for (size_t i = i0; i < n; ++i) {
        char axis;
        size_t value;
        sscanf(line[i], "fold along %c=%zu", &axis, &value);

        // flip at value along axis
        if (axis == 'y') {
            for (size_t y = value + 1; y < ny; ++y) {
                for (size_t x = 0; x < nx; ++x) {
                    if (grid[y][x]) {
                        grid[y][x] = 0;
                        grid[2 * value - y][x] = 1;
                    }
                }
            }

        } else if (axis == 'x') {
            for (size_t y = 0; y < ny; ++y) {
                for (size_t x = value + 1; x < nx; ++x) {
                    if (grid[y][x]) {
                        grid[y][x] = 0;
                        grid[y][2 * value - x] = 1;
                    }
                }
            }

        } else {
            assert(!"Illegal axis encountered.");
        }
    }
}

int main(void)
{
    // read input
    const char **line = 0;
    const size_t n_lines = lines_read(&line, "2021/input/13.txt");

    // find separator line
    const size_t i_sep = line_find(line, n_lines, "");

    // create grid
    size_t ny = 0;
    size_t nx = 0;
    for (size_t i = 0; i < i_sep; ++i) {
        size_t x, y;
        sscanf(line[i], "%zu,%zu", &x, &y);
        ny = MAX(ny, y + 1);
        nx = MAX(nx, x + 1);
    }
    int(*grid)[nx] = calloc(ny, sizeof(*grid));
    for (size_t i = 0; i < i_sep; ++i) {
        size_t x, y;
        sscanf(line[i], "%zu,%zu", &x, &y);
        grid[y][x] = 1;
    }

    // fold paper
    paper_fold(line, ny, nx, grid, i_sep + 1, i_sep + 2);

    // count dots
    size_t count = 0;
    for (size_t y = 0; y < ny; ++y) {
        for (size_t x = 0; x < nx; ++x) {
            count += grid[y][x];
        }
    }

    // part 1
    printf("%zu\n", count);

    // keep folding paper
    paper_fold(line, ny, nx, grid, i_sep + 2, n_lines);

    // part 2
    for (size_t y = 0; y < 6; ++y) {
        for (size_t x = 0; x < 40; ++x) {
            printf("%c", (grid[y][x] ? '#' : ' '));
        }
        printf("\n");
    }

    // cleanup
    lines_free(line, n_lines);
    free(grid);
}
