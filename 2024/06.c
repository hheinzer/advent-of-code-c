#include "aoc.h"

typedef struct {
    Vec2 pos;
    Vec2 dir;
} State;

State init(const Grid *grid);
int walk(const Grid *grid, Set *seen, const Vec2 *obstacle, Arena arena);

int main(void) {
    Arena arena = arena_create(4 << 20);

    Grid grid = grid_parse("2024/input/06.txt", &arena);

    Set seen = set_create(&arena);
    walk(&grid, &seen, 0, arena);
    printf("%ld\n", seen.length);

    long count = 0;
    set_remove(&seen, (Vec2[]){init(&grid).pos}, sizeof(Vec2));
    SetItem *item = set_items(&seen, &arena);
#pragma omp parallel
    {
        Arena thread = arena_thread(&arena);
#pragma omp for reduction(+ : count) schedule(auto)
        for (long i = 0; i < seen.length; i++) {
            count += walk(&grid, 0, item[i].key.data, thread);
        }
    }
    printf("%ld\n", count);

    arena_destroy(&arena);
}

State init(const Grid *grid) {
    State s = {0};
    s.pos = grid_find(grid, '^');
    s.dir = (Vec2){-1, 0};
    return s;
}

int walk(const Grid *grid, Set *seen, const Vec2 *obstacle, Arena arena) {
    State s = init(grid);
    Set path = set_create(&arena);
    while (grid_get(grid, s.pos.r, s.pos.c)) {
        if (seen) {
            set_insert(seen, &s.pos, sizeof(s.pos));
        }
        if (!seen) {
            set_insert(&path, &s, sizeof(State));
        }
        while (grid_get(grid, s.pos.r + s.dir.r, s.pos.c + s.dir.c) == '#' ||
               (obstacle && s.pos.r + s.dir.r == obstacle->r && s.pos.c + s.dir.c == obstacle->c)) {
            s.dir = (Vec2){s.dir.c, -s.dir.r};
        }
        s.pos = (Vec2){s.pos.r + s.dir.r, s.pos.c + s.dir.c};
        if (!seen && set_find(&path, &s, sizeof(State))) {
            return 1;
        }
    }
    return 0;
}
