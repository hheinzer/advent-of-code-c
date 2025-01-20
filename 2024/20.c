#include "aoc.h"

Dict distance(const Grid *grid, Arena *arena);
long cheat(const Dict *dist, const Vec2 *pos, const long *d0, long time);

int main(void) {
    Arena arena = arena_create(2 * mega_byte);

    Grid grid = grid_parse("2024/input/20.txt", &arena);
    Dict dist = distance(&grid, &arena);

    long part1 = 0;
    long part2 = 0;
    dict_for_each(item, &dist) {
        part1 += cheat(&dist, item->key.data, item->data, 2);
        part2 += cheat(&dist, item->key.data, item->data, 20);
    }
    printf("%ld\n", part1);
    printf("%ld\n", part2);

    arena_destroy(&arena);
}

Dict distance(const Grid *grid, Arena *arena) {
    Vec2 start = grid_find(grid, 'S');
    Dict dist = dict_create(arena, sizeof(long));
    dict_insert(&dist, &start, sizeof(Vec2), &(long){0});
    List queue = list_create(arena, sizeof(Vec2), nullptr);
    list_append(&queue, &start);
    while (queue.length) {
        Vec2 *cur = list_pop(&queue, 0);
        long *dst = dict_find(&dist, cur, sizeof(Vec2));
        if (grid_get(grid, cur->r, cur->c) == 'E') {
            break;
        }
        array_for_each(Vec2, dir, {-1, 0}, {+1, 0}, {0, -1}, {0, +1}) {
            Vec2 nxt = {cur->r + dir->r, cur->c + dir->c};
            if (grid_get(grid, nxt.r, nxt.c) != '#') {
                if (!dict_insert(&dist, &nxt, sizeof(Vec2), &(long){*dst + 1})) {
                    list_append(&queue, &nxt);
                }
            }
        }
    }
    return dist;
}

long cheat(const Dict *dist, const Vec2 *pos, const long *d0, long time) {
    long count = 0;
    for (long dr = -time; dr <= time; dr++) {
        for (long dc = -time; dc <= time; dc++) {
            long delta = labs(dr) + labs(dc);
            if (delta <= time) {
                long *d1 = dict_find(dist, &(Vec2){pos->r + dr, pos->c + dc}, sizeof(Vec2));
                if (d1 && *d1 - *d0 >= 100 + delta) {
                    count += 1;
                }
            }
        }
    }
    return count;
}
