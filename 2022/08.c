/*
 * Day 8: Treetop Tree House
 * (https://adventofcode.com/2022/day/8)
 *
 * Part 1:
 * - scan all for directions for the highest tree
 * - if for any direction the highest tree is smaller than the tree in question, it is
 *   visible
 *
 * Part 2:
 * - count the number of trees in every direction until there is on that has the same
 *   size or is larger
 */
#include "../aoc.h"

int is_visible(const char **line, size_t ni, size_t nj, size_t i0, size_t j0)
{
    // look left
    char max_height = line[i0][0];
    for (size_t j = 1; j < j0; ++j) {
        max_height = MAX(max_height, line[i0][j]);
    }
    if (line[i0][j0] > max_height) {
        return 1;
    }

    // look right
    max_height = line[i0][j0 + 1];
    for (size_t j = j0 + 2; j < nj; ++j) {
        max_height = MAX(max_height, line[i0][j]);
    }
    if (line[i0][j0] > max_height) {
        return 1;
    }

    // look up
    max_height = line[0][j0];
    for (size_t i = 1; i < i0; ++i) {
        max_height = MAX(max_height, line[i][j0]);
    }
    if (line[i0][j0] > max_height) {
        return 1;
    }

    // look down
    max_height = line[i0 + 1][j0];
    for (size_t i = i0 + 2; i < ni; ++i) {
        max_height = MAX(max_height, line[i][j0]);
    }
    if (line[i0][j0] > max_height) {
        return 1;
    }

    return 0;
}

size_t scenic_score(const char **line, size_t ni, size_t nj, size_t i0, size_t j0)
{
    // look left
    size_t score = 1;
    size_t j;
    for (j = 1; j <= j0; ++j) {
        if (line[i0][j0 - j] >= line[i0][j0]) {
            ++j;
            break;
        }
    }
    score *= j - 1;

    // look right
    for (j = 1; j < nj - j0; ++j) {
        if (line[i0][j0 + j] >= line[i0][j0]) {
            ++j;
            break;
        }
    }
    score *= j - 1;

    // look up
    size_t i;
    for (i = 1; i <= i0; ++i) {
        if (line[i0 - i][j0] >= line[i0][j0]) {
            ++i;
            break;
        }
    }
    score *= i - 1;

    // look down
    for (i = 1; i < ni - i0; ++i) {
        if (line[i0 + i][j0] >= line[i0][j0]) {
            ++i;
            break;
        }
    }
    score *= i - 1;

    return score;
}

int main(void)
{
    // read input
    const char **line = 0;
    const size_t n_lines = lines_read(&line, "08.txt");

    // find visible trees
    const size_t ni = n_lines;
    const size_t nj = strlen(line[0]);
    size_t n_visible = 2 * (ni + nj - 2); // add outside trees
    for (size_t i = 1; i < ni - 1; ++i) {
        for (size_t j = 1; j < nj - 1; ++j) {
            n_visible += is_visible(line, ni, nj, i, j);
        }
    }
    printf("%zu\n", n_visible);

    // find highest scenic score
    size_t max_score = 0;
    for (size_t i = 1; i < ni - 1; ++i) {
        for (size_t j = 1; j < nj - 1; ++j) {
            max_score = MAX(max_score, scenic_score(line, ni, nj, i, j));
        }
    }
    printf("%zu\n", max_score);

    // cleanup
    lines_free(line, n_lines);
}
