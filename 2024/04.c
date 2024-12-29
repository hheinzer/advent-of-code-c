#include "aoc.h"

typedef struct {
    long rows;
    long cols;
    char *data;
} Grid;

Grid parse(const char *fname, Arena *arena);
char at(Grid grid, long r, long c);
long search1(Grid grid, long r, long c);
long search2(Grid grid, long r, long c);

int main(void) {
    Arena arena = arena_create(1 << 20);

    Grid grid = parse("2024/input/04.txt", &arena);

    long part1 = 0;
    for (long r = 0; r < grid.rows; r++) {
        for (long c = 0; c < grid.cols; c++) {
            part1 += search1(grid, r, c);
        }
    }
    printf("%ld\n", part1);

    long part2 = 0;
    for (long r = 0; r < grid.rows; r++) {
        for (long c = 0; c < grid.cols; c++) {
            part2 += search2(grid, r, c);
        }
    }
    printf("%ld\n", part2);

    arena_destroy(&arena);
}

Grid parse(const char *fname, Arena *arena) {
    Grid grid = {0};
    grid.data = calloc(arena, grid.data, 1);
    FILE *file = fopen(fname, "r");
    assert(file);
    char line[256];
    while (fgets(line, sizeof(line), file)) {
        *strchr(line, '\n') = 0;
        grid.rows += 1;
        grid.cols = strlen(line);
        grid.data = realloc(arena, grid.data, grid.rows * grid.cols + 1);
        strncat(grid.data, line, grid.cols);
    }
    fclose(file);
    return grid;
}

char at(Grid grid, long r, long c) {
    if (r < 0 || grid.rows <= r || c < 0 || grid.cols <= c) {
        return 0;
    }
    return grid.data[r * grid.cols + c];
}

long search1(Grid grid, long r, long c) {
    if (at(grid, r, c) != 'X') {
        return 0;
    }
    long count = 0;
    char xmas[5] = {0};
    xmas[0] = at(grid, r, c);
    for (long dr = -1; dr <= 1; dr++) {
        for (long dc = -1; dc <= 1; dc++) {
            if (dr == 0 && dc == 0) {
                continue;
            }
            for (long i = 1; i <= 3; i++) {
                xmas[i] = at(grid, r + i * dr, c + i * dc);
            }
            if (!strcmp(xmas, "XMAS")) {
                count += 1;
            }
        }
    }
    return count;
}

long search2(Grid grid, long r, long c) {
    if (at(grid, r, c) != 'A') {
        return 0;
    }
    char s[5] = {0};
    s[0] = at(grid, r - 1, c - 1);
    s[1] = at(grid, r - 1, c + 1);
    s[2] = at(grid, r + 1, c + 1);
    s[3] = at(grid, r + 1, c - 1);
    return !strcmp(s, "MMSS") || !strcmp(s, "MSSM") || !strcmp(s, "SSMM") || !strcmp(s, "SMMS");
}
