#include "aoc.h"

void parse(List *number, const char *fname);
long evolve(Dict *price, long number, long n, Arena scratch);

int main(void) {
    Arena arena = arena_create(7 * mega_byte);
    Arena scratch = arena_scratch_create(&arena, mega_byte);

    List number = list_create(&arena, sizeof(long), nullptr);
    parse(&number, "2024/input/22.txt");

    long part1 = 0;
    Dict price = dict_create(&arena, sizeof(long));
    list_for_each(item, &number) {
        part1 += evolve(&price, *(long *)item->data, 2000, scratch);
    }
    printf("%ld\n", part1);

    long part2 = 0;
    dict_for_each(item, &price) {
        part2 = max(part2, *(long *)item->data);
    }
    printf("%ld\n", part2);

    arena_destroy(&arena);
}

void parse(List *number, const char *fname) {
    FILE *file = fopen(fname, "r");
    assert(file);
    char line[bufsize];
    while (fgets(line, sizeof(line), file)) {
        list_append(number, &(long){strtol(line, nullptr, decimal)});
    }
    fclose(file);
}

long evolve(Dict *price, long number, long n, Arena scratch) {
    Set seen = set_create(&scratch);
    List diff = list_create(&scratch, sizeof(long), nullptr);
    for (long i = 0; i < n; i++) {
        long next = number;
        next = ((next * 64) ^ next) % 16777216;
        next = ((next / 32) ^ next) % 16777216;
        next = ((next * 2048) ^ next) % 16777216;
        list_append(&diff, &(long){(next % 10) - (number % 10)});
        number = next;
        if (diff.length == 4) {
            long key[4] = {};
            long j = 0;
            list_for_each(item, &diff) {
                key[j++] = *(long *)item->data;
            }
            if (set_insert(&seen, key, sizeof(key))) {
                long *total = dict_insert(price, key, sizeof(key), &(long){number % 10});
                if (total) {
                    *total += number % 10;
                }
            }
            list_pop(&diff, 0);
        }
    }
    return number;
}
