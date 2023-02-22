/*
 * Day 5: Hydrothermal Venture
 * (https://adventofcode.com/2021/day/5)
 *
 * Part 1:
 * - use dict to represent grid
 * - add horizontal and vertical lines
 * - count overlapping points
 *
 * Part 2:
 * - add diagonal lines
 * - count overlapping points
 */
#include "aoc.h"

size_t count_overlapping(const Dict *grid)
{
    size_t count = 0;
    for (size_t i = 0; i < grid->size; ++i) {
        for (const Item *item = &grid->item[i]; item && item->key; item = item->next) {
            const size_t *_count = item->data;
            if (*_count >= 2) {
                ++count;
            }
        }
    }
    return count;
}

int main(void)
{
    // read input
    const char **line = 0;
    const size_t n_lines = lines_read(&line, "2021/input/05.txt");

    // build dict of points that are covered by line
    Dict *grid = dict_alloc(sizeof(size_t), 300000);
    char key[256] = "";
    for (size_t i = 0; i < n_lines; ++i) {
        long x0, y0, x1, y1;
        sscanf(line[i], "%ld,%ld -> %ld,%ld", &x0, &y0, &x1, &y1);
        if (x0 == x1) {
            for (long y = MIN(y0, y1); y <= MAX(y0, y1); ++y) {
                Item *item = dict_find(grid, KEY(key, "%ld,%ld", x0, y));
                if (item) {
                    size_t *count = item->data;
                    ++(*count);
                } else {
                    dict_insert(grid, key, memdup(&(size_t) { 1 }, sizeof(size_t)));
                }
            }

        } else if (y0 == y1) {
            for (long x = MIN(x0, x1); x <= MAX(x0, x1); ++x) {
                Item *item = dict_find(grid, KEY(key, "%ld,%ld", x, y0));
                if (item) {
                    size_t *count = item->data;
                    ++(*count);
                } else {
                    dict_insert(grid, key, memdup(&(size_t) { 1 }, sizeof(size_t)));
                }
            }
        }
    }

    // part 1
    printf("%zu\n", count_overlapping(grid));

    // add diagonal lines
    for (size_t i = 0; i < n_lines; ++i) {
        long x0, y0, x1, y1;
        sscanf(line[i], "%ld,%ld -> %ld,%ld", &x0, &y0, &x1, &y1);
        if ((x0 != x1) && (y0 != y1)) {
            long dx = x1 - x0;
            long dy = y1 - y0;
            assert(labs(dx) == labs(dy));
            for (long j = 0; j <= labs(dx); ++j) {
                const long x = x0 + j * SIGN(dx);
                const long y = y0 + j * SIGN(dy);
                Item *item = dict_find(grid, KEY(key, "%ld,%ld", x, y));
                if (item) {
                    size_t *count = item->data;
                    ++(*count);
                } else {
                    dict_insert(grid, key, memdup(&(size_t) { 1 }, sizeof(size_t)));
                }
            }
        }
    }

    // part 2
    printf("%zu\n", count_overlapping(grid));

    // cleanup
    lines_free(line, n_lines);
    dict_free(&grid, free);
}
