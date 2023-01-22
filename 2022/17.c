/*
 * Day 17: Pyroclastic Flow
 * (https://adventofcode.com/2022/day/17)
 *
 * Part 1:
 * - play tetris according to the rules of the puzzle
 * - to check for collisions with other rocks use dict for occupied coordinates
 *
 * Part 2:
 * - check for a repeating patter in the height increment
 * - if found, extrapolate from there the total height
 */
#include "aoc.h"

MINMAX(size_t)
MINMAX(long)

int does_collide(long r[5][2], const Dict *cave, long dx, long dy)
{
    // check for collision with wall, floor, or other rock
    char key[256] = "";
    for (size_t j = 0; j < 5; ++j) {
        const long rx = r[j][0] + dx;
        const long ry = r[j][1] + dy;
        if ((rx < 0) || (6 < rx)
            || (ry < 0)
            || dict_find(cave, KEY(key, "%ld,%ld", rx, ry))) {
            return 1;
        }
    }
    return 0;
}

int array_repeats(const long *a, const long *b, size_t n)
{
    for (size_t i = 0; i < n; ++i) {
        if (a[i] != b[i]) {
            return 0;
        }
    }
    return 1;
}

void find_pattern(const long *a, size_t n, size_t *np, size_t *nr)
{
    for (size_t i = 0; i < n; ++i) {
        // get pointer to sub array
        const long *s = a + i;

        // check if there are any repeating sections up to half of the length of s
        for (size_t r = 2; r < (n - i) / 2; ++r) {
            if (array_repeats(s, s + r, r)) {
                // make sure this repeats in the whole rest of s
                int all = 1;
                for (size_t j = 2 * r; j < (n - i) - r; j += r) {
                    all = all && array_repeats(s, s + j, r);
                    if (!all) {
                        break;
                    }
                }
                if (all) {
                    *np = i;
                    *nr = r;
                    return;
                }
            }
        }
    }
}

size_t drop_rocks(const char *jet, const long rock[5][5][2], size_t _n)
{
    // we are not dropping more than 5000 rocks
    const size_t n = min_size_t(_n, 5000);

    long *height = calloc(n + 1, sizeof(*height));
    Dict *cave = dict_alloc(0, 2 * (5 * n));
    const size_t njet = strlen(jet);
    size_t ijet = 0;
    char key[256] = "";
    size_t ret = 0;
    for (size_t i = 0; i < n; ++i) {
        // get current rock
        long r[5][2] = { 0 };
        memcpy(r, rock[i % 5], sizeof(r));
        for (size_t j = 0; j < 5; ++j) {
            r[j][1] += height[i] + 3;
        }

        // move rock until it lands
        while (1) {
            const long dx = (jet[(ijet++) % njet] == '<' ? -1 : +1);
            if (!does_collide(r, cave, dx, 0)) {
                for (size_t j = 0; j < 5; ++j) {
                    r[j][0] += dx;
                }
            }
            const long dy = -1;
            if (!does_collide(r, cave, 0, dy)) {
                for (size_t j = 0; j < 5; ++j) {
                    r[j][1] += dy;
                }
            } else {
                for (size_t j = 0; j < 5; ++j) {
                    dict_insert(cave, KEY(key, "%ld,%ld", r[j][0], r[j][1]), 0);
                }
                break;
            }
        }

        // determine new height
        for (size_t j = 0; j < 5; ++j) {
            height[i + 1] = max_long(height[i], r[j][1] + 1);
        }
    }

    if (n < _n) {
        // compute height increment
        long *dh = calloc(n, sizeof(*dh));
        for (size_t i = 0; i < n; ++i) {
            dh[i] = height[i + 1] - height[i];
        }

        // check for repeating pattern
        size_t np = 0;
        size_t nr = 0;
        find_pattern(dh, n, &np, &nr);
        assert((np > 0) && (nr > 0));

        // compute height of preamble
        long hp = 0;
        for (size_t i = 0; i < np; ++i) {
            hp += dh[i];
        }

        // compute height of repeat
        long hr = 0;
        for (size_t i = 0; i < nr; ++i) {
            hr += dh[np + i];
        }

        // compute final height
        ret = hp + hr * ((_n - np) / nr);
        for (size_t i = 0; i < (_n - np) % nr; ++i) {
            ret += dh[np + i];
        }

        // cleanup
        free(dh);
    } else {
        ret = height[n];
    }

    // cleanup
    free(height);
    dict_free(&cave, 0);

    return ret;
}

int main(void)
{
    // read input
    const char **line = 0;
    const size_t n_lines = lines_read(&line, "2022/input/17.txt");

    // set up jet
    const char *jet = line[0];

    // set up rocks (in initial position: offset two to the right),
    // last coordinate may be duplicate
    const long rock[5][5][2] = {
        { { 2, 0 }, { 3, 0 }, { 4, 0 }, { 5, 0 }, { 5, 0 } },
        { { 3, 0 }, { 2, 1 }, { 3, 1 }, { 4, 1 }, { 3, 2 } },
        { { 2, 0 }, { 3, 0 }, { 4, 0 }, { 4, 1 }, { 4, 2 } },
        { { 2, 0 }, { 2, 1 }, { 2, 2 }, { 2, 3 }, { 2, 3 } },
        { { 2, 0 }, { 3, 0 }, { 2, 1 }, { 3, 1 }, { 3, 1 } },
    };

    // part 1
    printf("%zu\n", drop_rocks(jet, rock, 2022));

    // part 2
    printf("%zu\n", drop_rocks(jet, rock, 1000000000000));

    // cleanup
    lines_free(line, n_lines);
}
