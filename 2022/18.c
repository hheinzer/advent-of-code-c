/*
 * Day 18: Boiling Boulders
 * (https://adventofcode.com/2022/day/18)
 *
 * Part 1:
 * - create 3D matrix of the cubes
 * - for every occupied cube
 *   - add 6 surface
 *   - remove 1 for every neighbor
 *
 * Part 2:
 * - flood fill all outside blocks
 * - do the same as in part 1, but only for outside blocks
 */
#include "aoc.h"

MINMAX(size_t)

void flood_fill(size_t ni, size_t nj, size_t nk,
    int outside[ni][nj][nk], int cube[ni][nj][nk],
    size_t i, size_t j, size_t k)
{
    // mark i,j,k as outside
    outside[i][j][k] = 1;

    // check all neighbors if they are also outside
    if ((i > 0) && !cube[i - 1][j][k] && !outside[i - 1][j][k]) {
        flood_fill(ni, nj, nk, outside, cube, i - 1, j, k);
    }
    if ((i < ni - 1) && !cube[i + 1][j][k] && !outside[i + 1][j][k]) {
        flood_fill(ni, nj, nk, outside, cube, i + 1, j, k);
    }
    if ((j > 0) && !cube[i][j - 1][k] && !outside[i][j - 1][k]) {
        flood_fill(ni, nj, nk, outside, cube, i, j - 1, k);
    }
    if ((j < nj - 1) && !cube[i][j + 1][k] && !outside[i][j + 1][k]) {
        flood_fill(ni, nj, nk, outside, cube, i, j + 1, k);
    }
    if ((k > 0) && !cube[i][j][k - 1] && !outside[i][j][k - 1]) {
        flood_fill(ni, nj, nk, outside, cube, i, j, k - 1);
    }
    if ((k < nk - 1) && !cube[i][j][k + 1] && !outside[i][j][k + 1]) {
        flood_fill(ni, nj, nk, outside, cube, i, j, k + 1);
    }
}

int main(void)
{
    // read input
    const char **line = 0;
    const size_t n_lines = lines_read(&line, "2022/input/18.txt");

    // get the maximum x-, y-, and z-coordinates
    size_t x_max = 0;
    size_t y_max = 0;
    size_t z_max = 0;
    for (size_t i = 0; i < n_lines; ++i) {
        size_t x, y, z;
        sscanf(line[i], "%zu,%zu,%zu", &x, &y, &z);
        x_max = max_size_t(x_max, x);
        y_max = max_size_t(y_max, y);
        z_max = max_size_t(z_max, z);
    }

    // create 3D occupation matrix (+2: buffer layer)
    const size_t ni = z_max + 1 + 2;
    const size_t nj = y_max + 1 + 2;
    const size_t nk = x_max + 1 + 2;
    int(*cube)[nj][nk] = calloc(ni, sizeof(*cube));
    for (size_t i = 0; i < n_lines; ++i) {
        size_t x, y, z;
        sscanf(line[i], "%zu,%zu,%zu", &x, &y, &z);
        cube[z + 1][y + 1][x + 1] = 1;
    }

    // compute total surface area
    size_t surf = 0;
    for (size_t i = 1; i < ni - 1; ++i) {
        for (size_t j = 1; j < nj - 1; ++j) {
            for (size_t k = 1; k < nk - 1; ++k) {
                if (cube[i][j][k]) {
                    surf += 6;
                    surf -= cube[i - 1][j][k];
                    surf -= cube[i + 1][j][k];
                    surf -= cube[i][j - 1][k];
                    surf -= cube[i][j + 1][k];
                    surf -= cube[i][j][k - 1];
                    surf -= cube[i][j][k + 1];
                }
            }
        }
    }
    printf("%zu\n", surf);

    // find all outside air cubes
    int(*outside)[nj][nk] = calloc(ni, sizeof(*outside));
    flood_fill(ni, nj, nk, outside, cube, 0, 0, 0);

    // recompute total surface area
    surf = 0;
    for (size_t i = 1; i < ni - 1; ++i) {
        for (size_t j = 1; j < nj - 1; ++j) {
            for (size_t k = 1; k < nk - 1; ++k) {
                if (cube[i][j][k]) {
                    surf += 6;
                    surf -= (outside[i - 1][j][k] ? cube[i - 1][j][k] : 1);
                    surf -= (outside[i + 1][j][k] ? cube[i + 1][j][k] : 1);
                    surf -= (outside[i][j - 1][k] ? cube[i][j - 1][k] : 1);
                    surf -= (outside[i][j + 1][k] ? cube[i][j + 1][k] : 1);
                    surf -= (outside[i][j][k - 1] ? cube[i][j][k - 1] : 1);
                    surf -= (outside[i][j][k + 1] ? cube[i][j][k + 1] : 1);
                }
            }
        }
    }
    printf("%zu\n", surf);

    // cleanup
    lines_free(line, n_lines);
    free(cube);
    free(outside);
}
