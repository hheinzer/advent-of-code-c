/*
 * Day 14: Regolith Reservoir
 * (https://adventofcode.com/2022/day/14)
 *
 * Part 1:
 * - create character buffer that is large enough to hold the grid (200x1000)
 * - mark all the stones
 * - drop sand according to rules, count how many were dropped
 *
 * Part 2:
 * - insert the ground and keep dropping sand
 */
#include "aoc.h"

MINMAX(long)

#define NY 200
#define NX 1000

size_t drop_sand(char grid[NY][NX], long max_y)
{
    long xs = 500;
    long ys = 0;
    size_t ns = 0;
    while ((ys < max_y) && grid[0][500] == '.') {
        if (grid[ys + 1][xs] == '.') { // fall down
            ++ys;
        } else if (grid[ys + 1][xs - 1] == '.') { // fall down left
            --xs;
            ++ys;
        } else if (grid[ys + 1][xs + 1] == '.') { // fall down right
            ++xs;
            ++ys;
        } else { // sand has landed
            grid[ys][xs] = 'o';
            xs = 500;
            ys = 0;
            ++ns;
        }
    }
    return ns;
}

int main(void)
{
    // read input
    const char **line = 0;
    const size_t n_lines = lines_read(&line, "2022/input/14.txt");

    // create reservoir
    long max_y = LONG_MIN;
    char grid[NY][NX];
    memset(grid, '.', sizeof(grid));
    for (size_t i = 0; i < n_lines; ++i) {
        char *buf = strcpy(malloc(strlen(line[i]) + 1), line[i]);
        char *tok = strtok(buf, "->");
        long x0 = 0;
        long y0 = 0;
        sscanf(tok, " %ld,%ld ", &x0, &y0);
        max_y = max_long(max_y, y0);
        while ((tok = strtok(0, "->"))) {
            long x1 = 0;
            long y1 = 0;
            sscanf(tok, " %ld,%ld ", &x1, &y1);
            max_y = max_long(max_y, y1);
            if (y1 == y0) { // add row
                const long nj = labs(x1 - x0);
                const long dx = (x1 - x0) / nj;
                for (long j = 0; j <= nj; ++j) {
                    grid[y0][x0 + j * dx] = '#';
                }
            } else { // add column
                const long ni = labs(y1 - y0);
                const long dy = (y1 - y0) / ni;
                for (long j = 0; j <= ni; ++j) {
                    grid[y0 + j * dy][x0] = '#';
                }
            }
            x0 = x1;
            y0 = y1;
        }
        free(buf);
    }

    // part 1
    size_t ns = drop_sand(grid, max_y);
    printf("%zu\n", ns);

    // insert floor
    max_y += 2;
    for (size_t j = 0; j < NX; ++j) {
        grid[max_y][j] = '#';
    }

    // part 2
    ns += drop_sand(grid, max_y);
    printf("%zu\n", ns);

    // cleanup
    lines_free(line, n_lines);
}
