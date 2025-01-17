#include "aoc.h"

long blink(long stone, long steps, Arena *arena);

int main(void) {
    Arena arena = arena_create(12 << 20);

    char *stones = string_parse("2024/input/11.txt", "\n", &arena);

    long part1 = 0;
    long part2 = 0;
    char *stone = strtok(stones, " ");
    while (stone) {
        part1 += blink(strtol(stone, 0, 10), 25, &arena);
        part2 += blink(strtol(stone, 0, 10), 75, &arena);
        stone = strtok(0, " ");
    }
    printf("%ld\n", part1);
    printf("%ld\n", part2);

    arena_destroy(&arena);
}

long blink(long stone, long steps, Arena *arena) {
    static Dict cache = {0};
    if (!cache.arena) {
        cache = dict_create(arena, sizeof(long));
    }
    if (steps == 0) {
        return 1;
    }
    long *cached = dict_find(&cache, (long[]){stone, steps}, sizeof(long[2]));
    if (cached) {
        return *cached;
    }
    long count = 0;
    long length = stone > 0 ? log10(stone) + 1 : 1;
    if (length % 2 == 0) {
        long divisor = pow(10, length / 2);
        long a = stone / divisor;
        long b = stone % divisor;
        count = blink(a, steps - 1, 0) + blink(b, steps - 1, 0);
    }
    else {
        count = blink(stone ? stone * 2024 : 1, steps - 1, 0);
    }
    dict_insert(&cache, (long[]){stone, steps}, sizeof(long[2]), &count);
    return count;
}
