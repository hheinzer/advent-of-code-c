#include "aoc.h"

void parse(List *byte, const char *fname);
long distance(const Grid *grid, Arena arena);

int main(void) {
    Arena arena = arena_create(mega_byte);

    List byte = list_create(&arena, sizeof(Vec2), nullptr);
    parse(&byte, "2024/input/18.txt");

    long size = 71;
    Grid grid = grid_create(size, size, '.', &arena);

    long count = 1024;
    list_for_each(item, &byte) {
        Vec2 *pos = item->data;
        grid_set(&grid, pos->r, pos->c, '#');
        if (--count == 0) {
            printf("%ld\n", distance(&grid, arena));
        }
        if (count < 0 && distance(&grid, arena) < 0) {
            printf("%ld,%ld\n", pos->c, pos->r);
            break;
        }
    }

    arena_destroy(&arena);
}

void parse(List *byte, const char *fname) {
    FILE *file = fopen(fname, "r");
    assert(file);
    long c;
    long r;
    while (fscanf(file, "%ld,%ld\n", &c, &r) == 2) {
        list_append(byte, &(Vec2){r, c});
    }
    fclose(file);
}

long distance(const Grid *grid, Arena arena) {
    Vec2 start = {0, 0};
    Vec2 end = {grid->rows - 1, grid->cols - 1};
    long(*dist)[grid->cols] = calloc(&arena, dist, grid->rows);
    List queue = list_create(&arena, sizeof(Vec2), nullptr);
    list_append(&queue, &start);
    while (queue.length) {
        Vec2 *cur = list_pop(&queue, 0);
        if (cur->r == end.r && cur->c == end.c) {
            return dist[cur->r][cur->c];
        }
        array_for_each(Vec2, dir, {-1, 0}, {+1, 0}, {0, -1}, {0, +1}) {
            Vec2 nxt = {cur->r + dir->r, cur->c + dir->c};
            if (grid_get(grid, nxt.r, nxt.c) != '.') {
                continue;
            }
            if (!dist[nxt.r][nxt.c]) {
                dist[nxt.r][nxt.c] = dist[cur->r][cur->c] + 1;
                list_append(&queue, &nxt);
            }
        }
    }
    return -1;
}
