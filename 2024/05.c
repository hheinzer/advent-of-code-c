#include "aoc.h"

void parse(Dict *rule, List *update, const char *fname, Arena *arena);
int compare(const void *_a, const void *_b, void *_rule);
int sorted(const List *page, Dict *rule);

int main(void) {
    Arena arena = arena_create(mega_byte);

    Dict rule = dict_create(&arena, sizeof(int));
    List update = list_create(&arena, sizeof(List), nullptr);
    parse(&rule, &update, "2024/input/05.txt", &arena);

    long part1 = 0;
    long part2 = 0;
    list_for_each(u, &update) {
        List *page = u->data;
        if (sorted(page, &rule)) {
            part1 += *(long *)list_get(page, page->length / 2);
        }
        else {
            list_sort(page, &rule);
            part2 += *(long *)list_get(page, page->length / 2);
        }
    }
    printf("%ld\n", part1);
    printf("%ld\n", part2);

    arena_destroy(&arena);
}

void parse(Dict *rule, List *update, const char *fname, Arena *arena) {
    FILE *file = fopen(fname, "r");
    assert(file);
    char line[bufsize];
    while (fgets(line, sizeof(line), file)) {
        if (line[0] == '\n') {
            break;
        }
        long a;
        long b;
        sscanf(line, "%ld|%ld", &a, &b);
        dict_insert(rule, (long[]){a, b}, sizeof(long[2]), &(int){-1});
        dict_insert(rule, (long[]){b, a}, sizeof(long[2]), &(int){+1});
    }
    while (fgets(line, sizeof(line), file)) {
        List page = list_create(arena, sizeof(long), compare);
        char *token = strtok(line, ",");
        while (token) {
            list_append(&page, &(long){strtol(token, nullptr, decimal)});
            token = strtok(nullptr, ",");
        }
        list_append(update, &page);
    }
    fclose(file);
}

int compare(const void *_a, const void *_b, void *_rule) {
    const long *a = _a;
    const long *b = _b;
    const Dict *rule = _rule;
    int *order = dict_find(rule, (long[]){*a, *b}, sizeof(long[2]));
    return order ? *order : 0;
}

int sorted(const List *page, Dict *rule) {
    for (auto a = page->begin, b = a->next; a && b; a = a->next, b = b->next) {
        if (compare(a->data, b->data, rule) > 0) {
            return 0;
        }
    }
    return 1;
}
