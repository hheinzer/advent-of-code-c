/*
 * Day 9: Smoke Basin
 * (https://adventofcode.com/2021/day/9)
 *
 * Part 1:
 * - create map with padding (= '9')
 * - go through every point check its four neighbors
 *
 * Part 2:
 * - keep track of which points have already been seen
 * - go through all points, if they have not been seen and they are smaller than '9',
 *   then recursively scan their neighbors with the same criteria
 * - mark seen tiles and count how many neighbors could be found
 */
#include "aoc.h"

CMP(size_t)

size_t compute_basin_size(const char *map, int *seen, size_t nj)
{
    if (*seen || *map >= '9') {
        return 0;
    } else {
        *seen = 1;
    }
    size_t size = 1;
    size += compute_basin_size(map - 1, seen - 1, nj); // left
    size += compute_basin_size(map + 1, seen + 1, nj); // right
    size += compute_basin_size(map - nj, seen - nj, nj); // up
    size += compute_basin_size(map + nj, seen + nj, nj); // down
    return size;
}

int main(void)
{
    // read input
    const char **line = 0;
    const size_t n_lines = lines_read(&line, "2021/input/09.txt");

    // create map with padding
    const size_t ni = n_lines + 2;
    const size_t nj = strlen(line[0]) + 2;
    char(*map)[nj] = calloc(ni, sizeof(*map));
    memset(map, '9', ni * sizeof(*map));
    for (size_t i = 0; i < ni - 2; ++i) {
        for (size_t j = 0; j < nj - 2; ++j) {
            map[i + 1][j + 1] = line[i][j];
        }
    }

    // compute risk level
    size_t risk = 0;
    for (size_t i = 1; i < ni - 1; ++i) {
        for (size_t j = 1; j < nj - 1; ++j) {
            int is_low_point = 1;
            is_low_point &= (map[i][j] < map[i][j - 1]); // left
            is_low_point &= (map[i][j] < map[i][j + 1]); // right
            is_low_point &= (map[i][j] < map[i - 1][j]); // up
            is_low_point &= (map[i][j] < map[i + 1][j]); // down
            if (is_low_point) {
                risk += 1 + map[i][j] - '0';
            }
        }
    }

    // part 1
    printf("%zu\n", risk);

    // compute basin sizes
    int(*seen)[nj] = calloc(ni, sizeof(*seen));
    List *size = list_alloc(sizeof(size_t));
    for (size_t i = 1; i < ni - 1; ++i) {
        for (size_t j = 1; j < nj - 1; ++j) {
            if (!seen[i][j] && (map[i][j] < '9')) {
                size_t _size = compute_basin_size(&map[i][j], &seen[i][j], nj);
                list_insert_last(size, memdup(&_size, sizeof(_size)));
            }
        }
    }

    // multiply sizes of three largest basins
    list_sort(size, cmp_size_t_dsc);
    size_t prod = 1;
    const Node *node = size->first;
    for (size_t i = 0; i < 3; ++i) {
        prod *= *(size_t *)node->data;
        node = node->next;
    }

    // part 2
    printf("%zu\n", prod);

    // cleanup
    lines_free(line, n_lines);
    free(map);
    free(seen);
    list_free(&size, free);
}
