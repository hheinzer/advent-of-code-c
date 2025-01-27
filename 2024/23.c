#include "aoc.h"

void parse(Dict *network, const char *fname, Arena *arena);
long search(const Dict *network, char *name, List *current, char *longest, Arena arena);

int main(void) {
    Arena arena = arena_create(mega_byte);

    Dict network = dict_create(&arena, sizeof(List));
    parse(&network, "2024/input/23.txt", &arena);

    long part1 = 0;
    char part2[bufsize] = "";
    dict_for_each(item, &network) {
        part1 += search(&network, item->key.data, nullptr, part2, arena);
    }
    printf("%ld\n", part1 / 3);
    printf("%s\n", part2);

    arena_destroy(&arena);
}

void add_edge(Dict *network, const char *name, const char *edge, Arena *arena) {
    List *connection = dict_find(network, name, 0);
    if (!connection) {
        List list = list_create(arena, 0, cmp_string);
        list_append(&list, strdup(arena, edge));
        dict_insert(network, name, 0, &list);
    }
    else {
        list_append(connection, strdup(arena, edge));
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

bool check1(const List *clique) {
    if (clique->length != 3) {
        return false;
    }
    list_for_each(item, clique) {
        char *name = item->data;
        if (name[0] == 't') {
            return true;
        }
    }
    return false;
}

void check2(List *clique, char *longest) {
    if (3 * clique->length - 1 > (long)strlen(longest)) {
        longest[0] = 0;
        list_sort(clique, nullptr);
        list_for_each(item, clique) {
            strcat(longest, item->data);
            strcat(longest, item->next ? "," : "");
        }
    }
}

long search(const Dict *network, char *name, List *current, char *longest, Arena arena) {
    long count = 0;
    if (!current) {
        List clique = list_create(&arena, 0, cmp_string);
        list_append(&clique, name);
        current = (List[]){clique, *(List *)dict_find(network, name, 0)};
    }
    List *clique = &current[0];
    List *candidates = &current[1];
    count += check1(clique);
    check2(clique, longest);
    list_for_each(item, candidates) {
        char *candidate = item->data;
        List *connection = dict_find(network, candidate, 0);
        List next_clique = list_clone(clique, &arena);
        list_append(&next_clique, candidate);
        List next_candidates = list_create(&arena, 0, nullptr);
        for (auto next = item->next; next; next = next->next) {
            if (list_find(connection, next->data)) {
                list_append(&next_candidates, next->data);
            }
        }
        count += search(network, candidate, (List[]){next_clique, next_candidates}, longest, arena);
    }
    return count;
}
