#include "aoc.h"

typedef struct {
    long r;
    long c;
} Vec2;

typedef struct {
    Vec2 pos;
    Vec2 dir;
} State;

State init(Grid grid);
int walk(Grid grid, Set *seen, Set *path);

int main(void) {
    Arena arena = arena_create(1 << 20);

    Grid grid = grid_parse("2024/input/06.txt", &arena);

    Set seen = set_create(&arena);
    walk(grid, &seen, 0);
    printf("%ld\n", seen.length);

    long count = 0;
    set_remove(&seen, (Vec2[]){init(grid).pos}, sizeof(Vec2));
    set_for_each(item, &seen) {
        Vec2 *pos = item->key.data;
        char old = grid_set(&grid, pos->r, pos->c, '#');
        Set path = set_create((Arena[]){arena});
        count += walk(grid, 0, &path);
        grid_set(&grid, pos->r, pos->c, old);
    }
    printf("%ld\n", count);

    arena_destroy(&arena);
}

State init(Grid grid) {
    for (long r = 0; r < grid.rows; r++) {
        for (long c = 0; c < grid.cols; c++) {
            if (grid_get(grid, r, c) == '^') {
                return (State){{r, c}, {-1, 0}};
            }
        }
    }
    abort();
}

int walk(Grid grid, Set *seen, Set *path) {
    State s = init(grid);
    while (grid_get(grid, s.pos.r, s.pos.c)) {
        if (seen) set_insert(seen, &s.pos, sizeof(s.pos));
        if (path) set_insert(path, &s, sizeof(s));
        while (grid_get(grid, s.pos.r + s.dir.r, s.pos.c + s.dir.c) == '#') {
            s.dir = (Vec2){s.dir.c, -s.dir.r};
        }
        s.pos = (Vec2){s.pos.r + s.dir.r, s.pos.c + s.dir.c};
        if (path && set_find(path, &s, sizeof(s))) {
            return 1;
        }
    }
    return 0;
}
