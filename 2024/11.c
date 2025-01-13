#include "aoc.h"

long count(const char *stone, long steps, Arena *arena);

int main(void) {
    Arena arena = arena_create(12 << 20);

    char *stones = string_parse("2024/input/11.txt", "\n", &arena);

    long part1 = 0;
    long part2 = 0;
    char *stone = strtok(stones, " ");
    while (stone) {
        part1 += count(stone, 25, &arena);
        part2 += count(stone, 75, &arena);
        stone = strtok(0, " ");
    }
    printf("%ld\n", part1);
    printf("%ld\n", part2);

    arena_destroy(&arena);
}

long cached_count(Dict *cache, long stone, long steps) {
    if (steps == 0) {
        return 1;
    }
    long *cached = dict_find(cache, (long[]){stone, steps}, sizeof(long[2]));
    if (cached) {
        return *cached;
    }
    long count = 0;
    long length = stone > 0 ? log10(stone) + 1 : 1;
    if (length % 2 == 0) {
        long divisor = pow(10, length / 2);
        long a = stone / divisor;
        long b = stone % divisor;
        count = cached_count(cache, a, steps - 1) + cached_count(cache, b, steps - 1);
    }
    else {
        count = cached_count(cache, stone ? stone * 2024 : 1, steps - 1);
    }
    dict_insert(cache, (long[]){stone, steps}, sizeof(long[2]), &count);
    return count;
}

long count(const char *stone, long steps, Arena *arena) {
    static Dict cache = {0};
    if (!cache.arena) {
        cache = dict_create(arena, sizeof(long));
    }
    return cached_count(&cache, strtol(stone, 0, 10), steps);
}
