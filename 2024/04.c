#include "aoc.h"

long search1(const Grid *grid, long r, long c);
long search2(const Grid *grid, long r, long c);

int main(void) {
    Arena arena = arena_create(1 << 20);

    Grid grid = grid_parse("2024/input/04.txt", &arena);

    long part1 = 0;
    long part2 = 0;
    for (long r = 0; r < grid.rows; r++) {
        for (long c = 0; c < grid.cols; c++) {
            part1 += search1(&grid, r, c);
            part2 += search2(&grid, r, c);
        }
    }
    printf("%ld\n", part1);
    printf("%ld\n", part2);

    arena_destroy(&arena);
}

long search1(const Grid *grid, long r, long c) {
    if (grid_get(grid, r, c) != 'X') {
        return 0;
    }
    long count = 0;
    char xmas[5] = {0};
    xmas[0] = grid_get(grid, r, c);
    for (long dr = -1; dr <= 1; dr++) {
        for (long dc = -1; dc <= 1; dc++) {
            if (dr == 0 && dc == 0) {
                continue;
            }
            for (long i = 1; i <= 3; i++) {
                xmas[i] = grid_get(grid, r + i * dr, c + i * dc);
            }
            if (!strcmp(xmas, "XMAS")) {
                count += 1;
            }
        }
    }
    return count;
}

long search2(const Grid *grid, long r, long c) {
    if (grid_get(grid, r, c) != 'A') {
        return 0;
    }
    char s[5] = {0};
    s[0] = grid_get(grid, r - 1, c - 1);
    s[1] = grid_get(grid, r - 1, c + 1);
    s[2] = grid_get(grid, r + 1, c + 1);
    s[3] = grid_get(grid, r + 1, c - 1);
    return !strcmp(s, "MMSS") || !strcmp(s, "MSSM") || !strcmp(s, "SSMM") || !strcmp(s, "SMMS");
}
