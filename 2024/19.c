#include "aoc.h"

void parse(List *pattern, List *design, const char *fname, Arena *arena);
long possible(const List *pattern, const char *design, Arena arena);

int main(void) {
    Arena arena = arena_create(mega_byte);

    List pattern = list_create(&arena, 0, nullptr);
    List design = list_create(&arena, 0, nullptr);
    parse(&pattern, &design, "2024/input/19.txt", &arena);

    long part1 = 0;
    long part2 = 0;
    list_for_each(item, &design) {
        long count = possible(&pattern, item->data, arena);
        part1 += count > 0;
        part2 += count;
    }
    printf("%ld\n", part1);
    printf("%ld\n", part2);

    arena_destroy(&arena);
}

void parse(List *pattern, List *design, const char *fname, Arena *arena) {
    FILE *file = fopen(fname, "r");
    assert(file);
    char line[256];
    while (fscanf(file, "%255[^,\n]%*[, ]", line)) {
        list_append(pattern, strdup(arena, line));
    }
    while (fgets(line, sizeof(line), file)) {
        *strchr(line, '\n') = 0;
        if (line[0]) {
            list_append(design, strdup(arena, line));
        }
    }
    fclose(file);
}

long possible(const List *pattern, const char *design, Arena arena) {
    static Dict cache = {};
    if (arena.data) {
        cache = dict_create(&arena, sizeof(long));
    }
    if (design[0] == 0) {
        return 1;
    }
    long *cached = dict_find(&cache, design, 0);
    if (cached) {
        return *cached;
    }
    long count = 0;
    list_for_each(item, pattern) {
        long length = strlen(item->data);
        if (!strncmp(item->data, design, length)) {
            count += possible(pattern, design + length, (Arena){});
        }
    }
    dict_insert(&cache, design, 0, &count);
    return count;
}
