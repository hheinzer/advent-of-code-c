#include "aoc.h"

typedef struct {
    Vec2 pos;
    Vec2 dir;
} State;

int walk(const Grid *grid, Dict *path, State s, const State *obstacle, Arena arena);

int main(void) {
    Arena arena = arena_create(4 << 20);

    Grid grid = grid_parse("2024/input/06.txt", &arena);

    State start = {0};
    start.pos = grid_find(&grid, '^');
    start.dir = (Vec2){-1, 0};

    Dict path = dict_create(&arena, sizeof(State));
    walk(&grid, &path, start, 0, arena);
    printf("%ld\n", path.length);

    long count = 0;
    dict_for_each(item, &path) {
        count += walk(&grid, 0, *(State *)item->data, item->key.data, arena);
    }
    printf("%ld\n", count);

    arena_destroy(&arena);
}

int walk(const Grid *grid, Dict *path, State s, const State *obstacle, Arena arena) {
    State prev = s;
    Set seen = set_create(&arena);
    while (grid_get(grid, s.pos.r, s.pos.c)) {
        if (path) {
            dict_insert(path, &s.pos, sizeof(Vec2), &prev);
            prev = s;
        }
        else {
            set_insert(&seen, &s, sizeof(State));
        }
        while (grid_get(grid, s.pos.r + s.dir.r, s.pos.c + s.dir.c) == '#' ||
               (obstacle && s.pos.r + s.dir.r == obstacle->pos.r &&
                s.pos.c + s.dir.c == obstacle->pos.c)) {
            s.dir = (Vec2){s.dir.c, -s.dir.r};
        }
        s.pos = (Vec2){s.pos.r + s.dir.r, s.pos.c + s.dir.c};
        if (!path && set_find(&seen, &s, sizeof(State))) {
            return 1;
        }
    }
    return 0;
}
