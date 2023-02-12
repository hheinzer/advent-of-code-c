/*
 * Day 12: Passage Pathing
 * (https://adventofcode.com/2021/day/12)
 *
 * Part 1:
 * - create adjacency matrix for the paths between the caves
 * - use dfs instead of bfs, to work on one path at a time
 *   - makes it possible to track how many times a cave was visited
 *
 * Part 2:
 * - revisit a seen caves once per path
 */
#include "aoc.h"

#define NL 10

int is_lowercase(const char *str)
{
    while (*str) {
        if (!islower(*str)) {
            return 0;
        }
        ++str;
    }
    return 1;
}

size_t dfs(size_t n, int small[n], int path[n][n], int seen[n], size_t i,
    size_t iS, size_t iE, int part)
{
    // return if end was found
    if (i == iE) {
        return 1;
    }

    // go to all adjacent caves, mark small ones as seen, don't revisit them, unless it's
    // part 2, then revisit once
    size_t count = 0;
    for (size_t j = 0; j < n; ++j) {
        if (path[i][j]) {
            if (!seen[j]) {
                seen[j] = (small[j] ? 1 : seen[j]);
                count += dfs(n, small, path, seen, j, iS, iE, part);
                seen[j] = (small[j] ? 0 : seen[j]);

            } else if ((j != iS) && (part == 2)) {
                count += dfs(n, small, path, seen, j, iS, iE, 1);
            }
        }
    }
    return count;
}

int main(void)
{
    // read input
    const char **line = 0;
    const size_t n_lines = lines_read(&line, "2021/input/12.txt");

    // create map between names of caves and indices
    Dict *cave = dict_alloc(sizeof(size_t), 1000);
    for (size_t i = 0, i_cave = 0; i < n_lines; ++i) {
        char name1[NL], name2[NL];
        sscanf(line[i], "%[^-]-%s", name1, name2);
        if (!dict_find(cave, name1)) {
            dict_insert(cave, name1, COPY((size_t) { i_cave++ }));
        }
        if (!dict_find(cave, name2)) {
            dict_insert(cave, name2, COPY((size_t) { i_cave++ }));
        }
    }

    // create list of small caves and path matrix
    const size_t n_caves = cave->len;
    int *small = calloc(n_caves, sizeof(*small));
    int(*path)[n_caves] = calloc(n_caves, sizeof(*path));
    for (size_t i = 0; i < n_lines; ++i) {
        char name1[NL], name2[NL];
        sscanf(line[i], "%[^-]-%s", name1, name2);
        const size_t i1 = *(size_t *)dict_find(cave, name1)->data;
        const size_t i2 = *(size_t *)dict_find(cave, name2)->data;
        small[i1] = is_lowercase(name1);
        small[i2] = is_lowercase(name2);
        path[i1][i2] = 1;
        path[i2][i1] = 1;
    }

    // set up depth first search
    const size_t iS = *(size_t *)dict_find(cave, "start")->data;
    const size_t iE = *(size_t *)dict_find(cave, "end")->data;
    int *seen = calloc(n_caves, sizeof(*seen));
    seen[iS] = 1;

    // part 1
    printf("%zu\n", dfs(n_caves, small, path, seen, iS, iS, iE, 1));

    // part 2
    printf("%zu\n", dfs(n_caves, small, path, seen, iS, iS, iE, 2));

    // cleanup
    lines_free(line, n_lines);
    dict_free(&cave, free);
    free(small);
    free(path);
    free(seen);
}
