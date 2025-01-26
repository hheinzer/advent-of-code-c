#include "aoc.h"

void parse(List *lock, List *key, const char *fname);
bool fits(const long *lock, const long *key);

int main(void) {
    Arena arena = arena_create(mega_byte);

    List lock = list_create(&arena, sizeof(long[5]), nullptr);
    List key = list_create(&arena, sizeof(long[5]), nullptr);
    parse(&lock, &key, "2024/input/25.txt");

    long part1 = 0;
    list_for_each(item1, &lock) {
        list_for_each(item2, &key) {
            part1 += fits(item1->data, item2->data);
        }
    }
    printf("%ld\n", part1);

    arena_destroy(&arena);
}

void parse(List *lock, List *key, const char *fname) {
    FILE *file = fopen(fname, "r");
    assert(file);
    char line[bufsize];
    while (fgets(line, sizeof(line), file)) {
        if (line[0] == '\n') {
            continue;
        }
        List *list = line[0] == '#' ? lock : key;
        long height[5] = {};
        for (long i = 0; i < 5; i++) {
            fgets(line, sizeof(line), file);
            for (long j = 0; j < 5; j++) {
                height[j] += line[j] == '#';
            }
        }
        list_append(list, height);
        fgets(line, sizeof(line), file);
    }
    fclose(file);
}

bool fits(const long *lock, const long *key) {
    for (long i = 0; i < 5; i++) {
        if (lock[i] + key[i] > 5) {
            return false;
        }
    }
    return true;
}
