#include "aoc.h"

static const Vec2 direction[] = {
    ['^'] = {-1, 0},
    ['>'] = {0, +1},
    ['v'] = {+1, 0},
    ['<'] = {0, -1},
};

char *parse(const char *fname, Arena *arena);
Grid expand(const Grid *grid, Arena *arena);
void check_move(Grid *grid, long move, Arena arena);
long sum(const Grid *grid);

int main(void) {
    Arena arena = arena_create(1 << 20);

    char *fname = "2024/input/15.txt";
    Grid grid = grid_parse(fname, &arena);
    Grid wide = expand(&grid, &arena);
    char *moves = parse(fname, &arena);

    for (char *move = moves; *move; move++) {
        check_move(&grid, *move, arena);
        check_move(&wide, *move, arena);
    }
    printf("%ld\n", sum(&grid));
    printf("%ld\n", sum(&wide));

    arena_destroy(&arena);
}

char *parse(const char *fname, Arena *arena) {
    FILE *file = fopen(fname, "r");
    assert(file);
    char line[256];
    while (fgets(line, sizeof(line), file)) {
        if (line[0] == '\n') {
            break;
        }
    }
    char *moves = calloc(arena, moves, 1);
    long n = 1;
    int c;
    while ((c = fgetc(file)) != EOF) {
        if (strchr("\n", c)) {
            continue;
        }
        moves = realloc(arena, moves, ++n);
        moves[n - 2] = c;
    }
    moves[n - 1] = 0;
    fclose(file);
    return moves;
}

Grid expand(const Grid *grid, Arena *arena) {
    Grid wide = grid_create(grid->rows, 2 * grid->cols, '#', arena);
    for (long r = 0; r < grid->rows; r++) {
        for (long c = 0; c < grid->cols; c++) {
            char chr = grid_get(grid, r, c);
            switch (chr) {
                case 'O':
                    grid_set(&wide, r, 2 * c, '[');
                    grid_set(&wide, r, 2 * c + 1, ']');
                    break;
                case '@':
                    grid_set(&wide, r, 2 * c, '@');
                    grid_set(&wide, r, 2 * c + 1, '.');
                    break;
                default:
                    grid_set(&wide, r, 2 * c, chr);
                    grid_set(&wide, r, 2 * c + 1, chr);
            }
        }
    }
    return wide;
}

void make_move(Grid *grid, SetItem *item, const Vec2 *dir) {
    if (item->next) {
        make_move(grid, item->next, dir);
    }
    Vec2 *pos = item->key.data;
    assert(grid_get(grid, pos->r + dir->r, pos->c + dir->c) == '.');
    char chr = grid_set(grid, pos->r, pos->c, '.');
    grid_set(grid, pos->r + dir->r, pos->c + dir->c, chr);
}

void check_move(Grid *grid, long move, Arena arena) {
    Vec2 dir = direction[move];
    Vec2 pos = grid_find(grid, '@');
    Set check = set_create(&arena);
    set_insert(&check, &pos, sizeof(Vec2));
    set_for_each(item, &check) {
        Vec2 *cur = item->key.data;
        switch (grid_get(grid, cur->r + dir.r, cur->c + dir.c)) {
            case '.':
                break;
            case 'O':
                set_insert(&check, &(Vec2){cur->r + dir.r, cur->c + dir.c}, sizeof(Vec2));
                break;
            case '[':
                set_insert(&check, &(Vec2){cur->r + dir.r, cur->c + dir.c}, sizeof(Vec2));
                set_insert(&check, &(Vec2){cur->r + dir.r, cur->c + dir.c + 1}, sizeof(Vec2));
                break;
            case ']':
                set_insert(&check, &(Vec2){cur->r + dir.r, cur->c + dir.c}, sizeof(Vec2));
                set_insert(&check, &(Vec2){cur->r + dir.r, cur->c + dir.c - 1}, sizeof(Vec2));
                break;
            case '#':
                return;
            default:
                abort();
        }
    }
    make_move(grid, check.begin, &dir);
}

long sum(const Grid *grid) {
    long sum = 0;
    for (long r = 0; r < grid->rows; r++) {
        for (long c = 0; c < grid->cols; c++) {
            char chr = grid_get(grid, r, c);
            if (chr == 'O' || chr == '[') {
                sum += 100 * r + c;
            }
        }
    }
    return sum;
}
