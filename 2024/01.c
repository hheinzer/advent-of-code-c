#include "aoc.h"

void parse(List *left, List *right, const char *fname);

int main(void) {
    Arena arena = arena_create(1 << 20);

    List left = list_create(&arena, sizeof(long), longcmp);
    List right = list_create(&arena, sizeof(long), longcmp);
    parse(&left, &right, "2024/input/01.txt");

    list_sort(&left, 0, 0);
    list_sort(&right, 0, 0);

    long part1 = 0;
    list_for_each_two(l, r, &left, &right) {
        long a = *(long *)l->data;
        long b = *(long *)r->data;
        part1 += labs(a - b);
    }
    printf("%ld\n", part1);

    long part2 = 0;
    list_for_each(l, &left) {
        list_for_each(r, &right) {
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

void parse(List *left, List *right, const char *fname) {
    FILE *file = fopen(fname, "r");
    assert(file);
    long a, b;
    while (fscanf(file, "%ld %ld", &a, &b) == 2) {
        list_append(left, &a);
        list_append(right, &b);
    }
    fclose(file);
}
