#include "aoc.h"

void parse(List *left, List *right, const char *fname);

int main(void) {
    Arena arena = arena_create(mega_byte);

    List left = list_create(&arena, sizeof(long), cmp_long);
    List right = list_create(&arena, sizeof(long), cmp_long);
    parse(&left, &right, "2024/input/01.txt");

    list_sort(&left, nullptr);
    list_sort(&right, nullptr);

    long part1 = 0;
    for (ListItem *l = left.begin, *r = right.begin; l && r; l = l->next, r = r->next) {
        long a = *(long *)l->data;
        long b = *(long *)r->data;
        part1 += labs(a - b);
    }
    printf("%ld\n", part1);

    long part2 = 0;
    list_for_each(l, &left) {
        long a = *(long *)l->data;
        list_for_each(r, &right) {
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
    long a;
    long b;
    while (fscanf(file, "%ld %ld\n", &a, &b) == 2) {
        list_append(left, &a);
        list_append(right, &b);
    }
    fclose(file);
}
