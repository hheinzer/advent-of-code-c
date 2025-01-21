#include "aoc.h"

void parse(Dict *network, const char *fname, Arena *arena);
void search(const Dict *network, const char *name, Set *cycle, Set *set, Arena arena);
int check(const char *cycle);
char *longest(const Set *cycle);

int main(void) {
    Arena arena = arena_create(23 * mega_byte);
    Arena scratch = arena_scratch_create(&arena, mega_byte);

    Dict network = dict_create(&arena, sizeof(Set));
    parse(&network, "2024/input/23.txt", &arena);

    Set cycle = set_create(&arena);
    dict_for_each(item, &network) {
        search(&network, item->key.data, &cycle, nullptr, scratch);
    }

    long part1 = 0;
    set_for_each(item, &cycle) {
        part1 += check(item->key.data);
    }
    printf("%ld\n", part1);
    printf("%s\n", longest(&cycle));

    arena_destroy(&arena);
}

void add_edge(Dict *network, const char *name, const char *edge, Arena *arena) {
    Set *connection = dict_find(network, name, 0);
    if (!connection) {
        Set set = set_create(arena);
        set_insert(&set, edge, 0);
        dict_insert(network, name, 0, &set);
    }
    else {
        set_insert(connection, edge, 0);
    }
}

void parse(Dict *network, const char *fname, Arena *arena) {
    FILE *file = fopen(fname, "r");
    assert(file);
    char name[2][3];
    while (fscanf(file, "%2s-%2s", name[0], name[1]) == 2) {
        add_edge(network, name[0], name[1], arena);
        add_edge(network, name[1], name[0], arena);
    }
    fclose(file);
}

int keycmp(const void *a, const void *b) {
    return memcmp(a, b, 2);
}

int set_insert_sorted(Set *cycle, const Set *set, Arena arena) {
    char(*key)[3] = calloc(&arena, key, set->length);
    long i = 0;
    set_for_each(item, set) {
        strncpy(key[i], item->key.data, 2);
        key[i++][2] = ',';
    }
    qsort(key, set->length, sizeof(*key), keycmp);
    key[set->length - 1][2] = 0;
    return set_insert(cycle, key, 0);
}

bool set_is_subset(const Set *self, const Set *other) {
    set_for_each(item, self) {
        if (!set_find(other, item->key.data, item->key.size)) {
            return false;
        }
    }
    return true;
}

void search(const Dict *network, const char *name, Set *cycle, Set *set, Arena arena) {
    if (!set) {
        set = (Set[]){set_create(&arena)};
        set_insert(set, name, 0);
    }
    if (set->length >= 3 && set_insert_sorted(cycle, set, arena)) {
        return;
    }
    Set *connection = dict_find(network, name, 0);
    set_for_each(item, connection) {
        char *neighbor = item->key.data;
        if (set_find(set, neighbor, 0)) {
            continue;
        }
        if (!set_is_subset(set, dict_find(network, neighbor, 0))) {
            continue;
        }
        Set clone = set_clone(set, &arena);
        set_insert(&clone, neighbor, 0);
        search(network, neighbor, cycle, &clone, arena);
    }
}

int check(const char *cycle) {
    long length = 0;
    long t_count = 0;
    for (const char *chr = cycle; *chr; chr += 3) {
        length += 1;
        t_count += *chr == 't';
    }
    return length == 3 && t_count > 0;
}

char *longest(const Set *cycle) {
    char *key = "";
    set_for_each(item, cycle) {
        if (strlen(item->key.data) > strlen(key)) {
            key = item->key.data;
        }
    }
    return key;
}
