#include "aoc.h"

typedef struct Pos Pos;
struct Pos {
    Vec2 pos;
    Vec2 dir;
    long score;
    Pos *prev;
};

int poscmp(const void *_a, const void *_b, void *);
long score(const Grid *grid, Set *seen, Arena *arena);

int main(void) {
    Arena arena = arena_create(27 << 20);

    Grid grid = grid_parse("2024/input/16.txt", &arena);

    Set seen = set_create(&arena);
    printf("%ld\n", score(&grid, &seen, &arena));

    printf("%ld\n", seen.length);

    arena_destroy(&arena);
}

int poscmp(const void *_a, const void *_b, void *) {
    const Pos *a = _a, *b = _b;
    return longcmp(&a->score, &b->score, 0);
}

long score(const Grid *grid, Set *seen, Arena *arena) {
    Pos start = {0};
    start.pos = grid_find(grid, 'S');
    start.dir = (Vec2){0, +1};
    long best = LONG_MAX;
    Dict scores = dict_create(arena, sizeof(long));
    Heap heap = heap_create(arena, sizeof(Pos), poscmp);
    heap_push(&heap, &start, 0);
    while (heap.length) {
        Pos *cur = heap_pop(&heap, 0);
        long *score = dict_insert(&scores, cur, sizeof(Vec2[2]), &cur->score);
        if (score && cur->score > *score) {
            continue;
        }
        if (grid_get(grid, cur->pos.r, cur->pos.c) == 'E' && cur->score <= best) {
            best = cur->score;
            for (Pos *pos = cur; pos; pos = pos->prev) {
                set_insert(seen, &pos->pos, sizeof(Vec2));
            }
        }
        array_for_each(Vec2, dir, cur->dir, {-cur->dir.c, cur->dir.r}, {cur->dir.c, -cur->dir.r}) {
            Pos nxt = {0};
            nxt.pos = (Vec2){cur->pos.r + dir->r, cur->pos.c + dir->c};
            if (grid_get(grid, nxt.pos.r, nxt.pos.c) == '#') {
                continue;
            }
            nxt.dir = *dir;
            nxt.score = cur->score + (!memcmp(&cur->dir, dir, sizeof(Vec2)) ? 1 : 1001);
            nxt.prev = cur;
            heap_push(&heap, &nxt, 0);
        }
    }
    return best;
}
