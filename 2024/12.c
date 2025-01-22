#include "aoc.h"

void create(List *regions, const Grid *grid, Arena *arena);
long price1(const Set *region, const Grid *grid);
long price2(const Set *region);

int main(void) {
    Arena arena = arena_create(8 * mega_byte);

    Grid grid = grid_parse("2024/input/12.txt", &arena);

    List regions = list_create(&arena, sizeof(Set), nullptr);
    create(&regions, &grid, &arena);

    long part1 = 0;
    long part2 = 0;
    list_for_each(region, &regions) {
        part1 += price1(region->data, &grid);
        part2 += price2(region->data);
    }
    printf("%ld\n", part1);
    printf("%ld\n", part2);

    arena_destroy(&arena);
}

void create(List *regions, const Grid *grid, Arena *arena) {
    Set seen = set_create(arena);
    for (long r = 0; r < grid->rows; r++) {
        for (long c = 0; c < grid->cols; c++) {
            if (set_insert(&seen, &(Vec2){r, c}, sizeof(Vec2))) {
                continue;
            }
            char plant = grid_get(grid, r, c);
            Set region = set_create(arena);
            List queue = list_create(arena, sizeof(Vec2), nullptr);
            list_append(&queue, &(Vec2){r, c});
            while (queue.length) {
                Vec2 *cur = list_pop(&queue, 0);
                if (set_insert(&region, cur, sizeof(Vec2))) {
                    continue;
                }
                set_insert(&seen, cur, sizeof(Vec2));
                array_for_each(Vec2, dir, {-1, 0}, {+1, 0}, {0, -1}, {0, +1}) {
                    Vec2 next = {cur->r + dir->r, cur->c + dir->c};
                    if (grid_get(grid, next.r, next.c) == plant) {
                        list_append(&queue, &next);
                    }
                }
            }
            list_append(regions, &region);
        }
    }
}

long price1(const Set *region, const Grid *grid) {
    long perimeter = 0;
    set_for_each(r2, region) {
        Vec2 *cur = r2->key.data;
        array_for_each(Vec2, dir, {-1, 0}, {+1, 0}, {0, -1}, {0, +1}) {
            Vec2 next = {cur->r + dir->r, cur->c + dir->c};
            if (grid_get(grid, next.r, next.c) != grid_get(grid, cur->r, cur->c)) {
                perimeter += 1;
            }
        }
    }
    return region->length * perimeter;
}

long price2(const Set *region) {
    long corners = 0;
    set_for_each(r2, region) {
        Vec2 *cur = r2->key.data;
        array_for_each(Vec2, dir, {1, 1}, {1, -1}, {-1, 1}, {-1, -1}) {
            Vec2 next_in_row = {cur->r + dir->r, cur->c};
            Vec2 next_in_col = {cur->r, cur->c + dir->c};
            Vec2 next_diagonal = {cur->r + dir->r, cur->c + dir->c};
            if (!set_find(region, &next_in_row, sizeof(Vec2)) &&
                !set_find(region, &next_in_col, sizeof(Vec2))) {
                corners += 1;
            }
            if (set_find(region, &next_in_row, sizeof(Vec2)) &&
                set_find(region, &next_in_col, sizeof(Vec2)) &&
                !set_find(region, &next_diagonal, sizeof(Vec2))) {
                corners += 1;
            }
        }
    }
    return region->length * corners;
}
