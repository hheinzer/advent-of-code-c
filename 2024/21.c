#include "aoc.h"

void parse(List *code, const char *fname, Arena *arena);
Dict numpad(Arena *arena);
Dict dirpad(Arena *arena);
long complexity(const char *code, const Dict *num, const Dict *dir, long robots, Arena *arena);

int main(void) {
    Arena arena = arena_create(mega_byte);

    List code = list_create(&arena, 0, nullptr);
    parse(&code, "2024/input/21.txt", &arena);

    Dict num = numpad(&arena);
    Dict dir = dirpad(&arena);

    long part1 = 0;
    long part2 = 0;
    list_for_each(item, &code) {
        part1 += complexity(item->data, &num, &dir, 2, &arena);
        part2 += complexity(item->data, &num, &dir, 25, &arena);
    }
    printf("%ld\n", part1);
    printf("%ld\n", part2);

    arena_destroy(&arena);
}

void parse(List *code, const char *fname, Arena *arena) {
    FILE *file = fopen(fname, "r");
    assert(file);
    char line[bufsize];
    while (fgets(line, sizeof(line), file)) {
        *strchr(line, '\n') = 0;
        list_append(code, strdup(arena, line));
    }
    fclose(file);
}

typedef struct {
    Vec2 pos;
    char chr;
    void *prev;
} State;

typedef struct {
    char str[9][7];
    long count;
} Path;

long distance(const State *state) {
    return !!state->chr + (state->prev ? distance(state->prev) : 0);
}

void reverse_append(char *path, const State *state) {
    if (state->prev) {
        reverse_append(path, state->prev);
    }
    strncat(path, &state->chr, 1);
}

typedef struct {
    long r;
    long c;
    char chr;
} Dir;

void find(Path *path, const Grid *grid, char start, char end, Arena arena) {
    State state = {.pos = grid_find(grid, start)};
    List queue = list_create(&arena, sizeof(State), nullptr);
    list_append(&queue, &state);
    long best = LONG_MAX;
    while (queue.length) {
        State *cur = list_pop(&queue, 0);
        if (grid_get(grid, cur->pos.r, cur->pos.c) == end) {
            long dist = distance(cur);
            if (dist <= best) {
                best = dist;
                reverse_append(path->str[path->count], cur);
                strcat(path->str[path->count], "A");
                path->count += 1;
                continue;
            }
            return;
        }
        array_for_each(Dir, dir, {-1, 0, '^'}, {+1, 0, 'v'}, {0, -1, '<'}, {0, +1, '>'}) {
            State nxt = {.pos = {cur->pos.r + dir->r, cur->pos.c + dir->c}};
            if (!grid_get(grid, nxt.pos.r, nxt.pos.c)) {
                continue;
            }
            nxt.chr = dir->chr;
            nxt.prev = cur;
            list_append(&queue, &nxt);
        }
    }
}

Dict numpad(Arena *arena) {
    Grid grid = {.rows = 4, .cols = 3};
    grid.data = strdup(arena, "789456123#0A");
    *strchr(grid.data, '#') = 0;
    Dict paths = dict_create(arena, sizeof(Path));
    for (const char *a = "A0123456789"; *a; a++) {
        for (const char *b = "A0123456789"; *b; b++) {
            Path path = {};
            find(&path, &grid, *a, *b, *arena);
            dict_insert(&paths, (char[]){*a, *b}, sizeof(char[2]), &path);
        }
    }
    return paths;
}

Dict dirpad(Arena *arena) {
    Grid grid = {.rows = 2, .cols = 3};
    grid.data = strdup(arena, "#^A<v>");
    *strchr(grid.data, '#') = 0;
    Dict paths = dict_create(arena, sizeof(Path));
    for (const char *a = "A<v>^"; *a; a++) {
        for (const char *b = "A<v>^"; *b; b++) {
            Path path = {};
            find(&path, &grid, *a, *b, *arena);
            dict_insert(&paths, (char[]){*a, *b}, sizeof(char[2]), &path);
        }
    }
    return paths;
}

List generate(const char *code, const Dict *pad, Arena *arena) {
    List seq = list_create(arena, 0, nullptr);
    for (const char *a = "A", *b = code; *b; a = b, b++) {
        Path *path = dict_find(pad, (char[]){*a, *b}, sizeof(char[2]));
        long length = seq.length;
        if (length == 0) {
            for (long i = 0; i < path->count; i++) {
                list_append(&seq, strdup(arena, path->str[i]));
            }
        }
        else {
            for (ListItem *item = seq.begin; length--; item = item->next) {
                char *str = item->data;
                item->data = strapp(arena, (char *)strdup(arena, str), path->str[0]);
                for (long i = 1; i < path->count; i++) {
                    list_append(&seq, strapp(arena, (char *)strdup(arena, str), path->str[i]));
                }
            }
        }
    }
    return seq;
}

typedef struct {
    long depth;
    char code[];
} Cache;

long get_length(const char *code, const Dict *dir, long depth, Arena *arena) {
    static Dict cache = {};
    if (!cache.arena) {
        cache = dict_create(arena, sizeof(long));
    }
    long size = sizeof(Cache) + sizeof(char[strlen(code) + 1]);
    Cache *key = arena_malloc(arena, 1, size, alignof(Cache));
    key->depth = depth;
    strcpy(key->code, code);
    long *cached = dict_find(&cache, key, size);
    if (cached) {
        return *cached;
    }
    if (depth == 0) {
        return strlen(code);
    }
    long length = 0;
    for (const char *a = "A", *b = code; *b; a = b, b++) {
        Path *path = dict_find(dir, (char[]){*a, *b}, sizeof(char[2]));
        long len = LONG_MAX;
        for (long i = 0; i < path->count; i++) {
            len = min(len, get_length(path->str[i], dir, depth - 1, arena));
        }
        length += len;
    }
    dict_insert(&cache, key, size, &length);
    return length;
}

long complexity(const char *code, const Dict *num, const Dict *dir, long robots, Arena *arena) {
    List seq = generate(code, num, arena);
    long length = LONG_MAX;
    list_for_each(item, &seq) {
        length = min(length, get_length(item->data, dir, robots, arena));
    }
    return strtol(code, nullptr, decimal) * length;
}
