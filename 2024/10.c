#include "aoc.h"

typedef struct {
    long r;
    long c;
} Vec2;

long score(const Grid *grid, long r, long c, int all, Arena arena);

int main(void) {
    Arena arena = arena_create(1 << 20);

    Grid grid = grid_parse("2024/input/10.txt", &arena);

    long part1 = 0;
    for (long r = 0; r < grid.rows; r++) {
        for (long c = 0; c < grid.cols; c++) {
            if (grid_get(&grid, r, c) == '0') {
                part1 += score(&grid, r, c, 0, arena);
            }
        }
    }
    printf("%ld\n", part1);

    long part2 = 0;
    for (long r = 0; r < grid.rows; r++) {
        for (long c = 0; c < grid.cols; c++) {
            if (grid_get(&grid, r, c) == '0') {
                part2 += score(&grid, r, c, 1, arena);
            }
        }
    }
    printf("%ld\n", part2);

    arena_destroy(&arena);
}

long score(const Grid *grid, long r, long c, int all, Arena arena) {
    long count = 0;
    List queue = list_create(&arena, sizeof(Vec2), 0);
    Set seen = set_create(&arena);
    list_append(&queue, &(Vec2){r, c});
    set_insert(&seen, &(Vec2){r, c}, sizeof(Vec2));
    while (queue.length) {
        const Vec2 *curr = list_pop(&queue, 0);
        if (grid_get(grid, curr->r, curr->c) == '9') {
            count += 1;
            continue;
        }
        Vec2 delta[4] = {{-1, 0}, {+1, 0}, {0, -1}, {0, +1}};
        for (Vec2 *d = delta; d < delta + 4; d++) {
            Vec2 next = {curr->r + d->r, curr->c + d->c};
            if (grid_get(grid, next.r, next.c) - grid_get(grid, curr->r, curr->c) != 1) {
                continue;
            }
            if (all || !set_insert(&seen, &next, sizeof(Vec2))) {
                list_append(&queue, &next);
            }
        }
    }
    return count;
}
