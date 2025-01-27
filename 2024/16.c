#include "aoc.h"

typedef struct Pos Pos;
struct Pos {
    long score;
    Vec2 pos;
    Vec2 dir;
    Pos *prev;
};

long lowest_score(const Grid *grid, Set *seen, Arena *arena);

int main(void) {
    Arena arena = arena_create(32 * mega_byte);

    Grid grid = grid_parse("2024/input/16.txt", &arena);

    Set seen = set_create(&arena);
    printf("%ld\n", lowest_score(&grid, &seen, &arena));
    printf("%ld\n", seen.length);

    arena_destroy(&arena);
}

long lowest_score(const Grid *grid, Set *seen, Arena *arena) {
    Pos start = {.pos = grid_find(grid, 'S'), .dir = {0, +1}};
    long best = LONG_MAX;
    Dict scores = dict_create(arena, sizeof(long));
    Heap heap = heap_create(arena, sizeof(Pos), cmp_long);
    heap_push(&heap, &start, nullptr);
    while (heap.length) {
        Pos *cur = heap_pop(&heap, nullptr);
        long *score = dict_insert(&scores, &cur->pos, sizeof(Vec2[2]), &cur->score);
        if (score && *score < cur->score) {
            continue;
        }
        if (grid_get(grid, cur->pos.r, cur->pos.c) == 'E') {
            best = cur->score;
            for (auto pos = cur; pos; pos = pos->prev) {
                set_insert(seen, &pos->pos, sizeof(Vec2));
            }
        }
        array_for_each(Vec2, dir, cur->dir, {-cur->dir.c, cur->dir.r}, {cur->dir.c, -cur->dir.r}) {
            Pos nxt = {.pos = {cur->pos.r + dir->r, cur->pos.c + dir->c}};
            if (grid_get(grid, nxt.pos.r, nxt.pos.c) == '#') {
                continue;
            }
            nxt.dir = *dir;
            nxt.score = cur->score + (!memcmp(&cur->dir, dir, sizeof(Vec2)) ? 1 : 1001);
            nxt.prev = cur;
            heap_push(&heap, &nxt, nullptr);
        }
    }
    return best;
}
