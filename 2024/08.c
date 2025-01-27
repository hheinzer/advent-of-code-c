#include "aoc.h"

Dict freqs_find(const Grid *grid, Arena *arena);
long count(const Grid *grid, const Dict *freq, long start, long stop, Arena arena);

int main(void) {
    Arena arena = arena_create(mega_byte);

    Grid grid = grid_parse("2024/input/08.txt", &arena);
    Dict freq = freqs_find(&grid, &arena);

    printf("%ld\n", count(&grid, &freq, 1, 1, arena));
    printf("%ld\n", count(&grid, &freq, 0, 0, arena));

    arena_destroy(&arena);
}

Dict freqs_find(const Grid *grid, Arena *arena) {
    Dict freq = dict_create(arena, sizeof(List));
    for (auto f = grid->data; *f; f++) {
        if (*f == '.' || dict_find(&freq, f, sizeof(*f))) {
            continue;
        }
        List pos = list_create(arena, sizeof(Vec2), nullptr);
        for (long r = 0; r < grid->rows; r++) {
            for (long c = 0; c < grid->cols; c++) {
                if (grid_get(grid, r, c) == *f) {
                    list_append(&pos, &(Vec2){r, c});
                }
            }
        }
        dict_insert(&freq, f, sizeof(*f), &pos);
    }
    return freq;
}

long count(const Grid *grid, const Dict *freq, long start, long stop, Arena arena) {
    Set loc = set_create(&arena);
    dict_for_each(f, freq) {
        List *pos = f->data;
        list_for_each(p1, pos) {
            list_for_each(p2, pos) {
                if (p1 == p2) {
                    continue;
                }
                Vec2 *a = p1->data;
                Vec2 *b = p2->data;
                Vec2 dir = {b->r - a->r, b->c - a->c};
                Vec2 l1 = *a;
                Vec2 l2 = *b;
                long n = start;
                while (grid_get(grid, l1.r, l1.c) || grid_get(grid, l2.r, l2.c)) {
                    l1 = (Vec2){a->r - (n * dir.r), a->c - (n * dir.c)};
                    l2 = (Vec2){b->r + (n * dir.r), b->c + (n * dir.c)};
                    if (grid_get(grid, l1.r, l1.c)) {
                        set_insert(&loc, &l1, sizeof(l1));
                    }
                    if (grid_get(grid, l2.r, l2.c)) {
                        set_insert(&loc, &l2, sizeof(l2));
                    }
                    if (stop && n >= stop) {
                        break;
                    }
                    n += 1;
                }
            }
        }
    }
    return loc.length;
}
