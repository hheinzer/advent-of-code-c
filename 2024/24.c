#include "aoc.h"

typedef enum { NONE, AND, XOR, OR } GateType;

typedef struct {
    GateType type;
    char *input[2];
    long value;
} Gate;

void parse(Dict *network, const char *fname, Arena *arena);
void compute(const Dict *network, Gate *gate);
Set swapped(const Dict *network, Arena *arena);

int main(void) {
    Arena arena = arena_create(mega_byte);

    Dict network = dict_create(&arena, sizeof(Gate));
    parse(&network, "2024/input/24.txt", &arena);

    long part1 = 0;
    dict_for_each(item, &network) {
        char *name = item->key.data;
        Gate *gate = item->data;
        compute(&network, gate);
        switch (name[0]) {
            case 'z': part1 |= gate->value << strtol(name + 1, nullptr, decimal); break;
            default: continue;
        }
    }
    printf("%ld\n", part1);

    Set wrong = swapped(&network, &arena);
    List list = list_create(&arena, 0, cmp_string);
    set_for_each(item, &wrong) {
        list_append(&list, item->key.data);
    }
    list_sort(&list, nullptr);
    list_for_each(item, &list) {
        printf("%s%c", (char *)item->data, item->next ? ',' : '\n');
    }

    arena_destroy(&arena);
}

void parse(Dict *network, const char *fname, Arena *arena) {
    FILE *file = fopen(fname, "r");
    assert(file);
    char line[bufsize];
    while (fgets(line, sizeof(line), file)) {
        *strchr(line, '\n') = 0;
        if (!line[0]) {
            break;
        }
        char *colon = strchr(line, ':');
        Gate gate = {};
        gate.value = strtol(colon + 1, nullptr, decimal);
        *colon = 0;
        dict_insert(network, line, 0, &gate);
    }
    while (fgets(line, sizeof(line), file)) {
        *strchr(line, '\n') = 0;
        char *token = strtok(line, " ->");
        Gate gate = {};
        gate.value = -1;
        gate.input[0] = strdup(arena, token);
        token = strtok(nullptr, " ->");
        switch (token[0]) {
            case 'A': gate.type = AND; break;
            case 'X': gate.type = XOR; break;
            case 'O': gate.type = OR; break;
            default: unreachable();
        }
        token = strtok(nullptr, " ->");
        gate.input[1] = strdup(arena, token);
        token = strtok(nullptr, " ->");
        dict_insert(network, token, 0, &gate);
    }
    fclose(file);
}

void compute(const Dict *network, Gate *gate) {
    if (gate->value != -1) {
        return;
    }
    Gate *input[2];
    input[0] = dict_find(network, gate->input[0], 0);
    input[1] = dict_find(network, gate->input[1], 0);
    compute(network, input[0]);
    compute(network, input[1]);
    if (input[0]->value != -1 && input[1]->value != -1) {
        switch (gate->type) {
            case AND: gate->value = input[0]->value & input[1]->value; break;
            case XOR: gate->value = input[0]->value ^ input[1]->value; break;
            case OR: gate->value = input[0]->value | input[1]->value; break;
            default: unreachable();
        }
    }
}

char *largest_bit(const Dict *network) {
    char *zmax = "z00";
    dict_for_each(item, network) {
        char *name = item->key.data;
        if (name[0] == 'z' &&
            strtol(name + 1, nullptr, decimal) > strtol(zmax + 1, nullptr, decimal)) {
            zmax = name;
        }
    }
    return zmax;
}

Set swapped(const Dict *network, Arena *arena) {
    Set wrong = set_create(arena);
    char *zmax = largest_bit(network);
    dict_for_each(a, network) {
        char *a_name = a->key.data;
        Gate *a_gate = a->data;
        char **a_input = a_gate->input;
        if (!a_input[0] || !a_input[1]) {
            continue;
        }
        if (a_name[0] == 'z' && a_gate->type != XOR && strcmp(a_name, zmax)) {
            set_insert(&wrong, a_name, 0);
        }
        if (a_gate->type == XOR && !strchr("xyz", a_name[0]) && !strchr("xyz", a_input[0][0]) &&
            !strchr("xyz", a_input[1][0])) {
            set_insert(&wrong, a_name, 0);
        }
        if (a_gate->type == AND && strcmp("x00", a_input[0]) && strcmp("x00", a_input[1])) {
            dict_for_each(b, network) {
                Gate *b_gate = b->data;
                char **b_input = b_gate->input;
                if (b_input[0] && b_input[1] &&
                    (!strcmp(a_name, b_input[0]) || !strcmp(a_name, b_input[1])) &&
                    b_gate->type != OR) {
                    set_insert(&wrong, a_name, 0);
                }
            }
        }
        if (a_gate->type == XOR) {
            dict_for_each(b, network) {
                Gate *b_gate = b->data;
                char **b_input = b_gate->input;
                if (b_input[0] && b_input[1] &&
                    (!strcmp(a_name, b_input[0]) || !strcmp(a_name, b_input[1])) &&
                    b_gate->type == OR) {
                    set_insert(&wrong, a_name, 0);
                }
            }
        }
    }
    return wrong;
}
