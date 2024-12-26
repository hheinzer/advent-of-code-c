#include "aoc.h"

int main(void) {
    Arena arena = arena_create(1 << 20);

    List left = list_create(&arena, sizeof(long), longcmp);
    List right = list_create(&arena, sizeof(long), longcmp);
    FILE *file = fopen("2024/input/01.txt", "r");
    assert(file);
    while (true) {
        long a, b;
        if (fscanf(file, "%ld %ld", &a, &b) != 2) {
            break;
        }
        list_append(&left, &a);
        list_append(&right, &b);
    }
    fclose(file);

    list_sort(&left, 0);
    list_sort(&right, 0);

    long part1 = 0;
    ListForEach2(l, r, &left, &right) {
        long a = *(long *)l->data;
        long b = *(long *)r->data;
        part1 += labs(a - b);
    }
    printf("%ld\n", part1);

    long part2 = 0;
    ListForEach(l, &left) {
        ListForEach(r, &right) {
            long a = *(long *)l->data;
            long b = *(long *)r->data;
            if (a == b) {
                part2 += a;
            }
        }
    }
    printf("%ld\n", part2);

    arena_destroy(&arena);
}
