#include "aoc.h"

typedef struct {
    long test;
    long *number;
    long length;
} Equation;

void parse(List *eqns, const char *fname, Arena *arena);
long solvable(const Equation *eqn, long res, long i, int part2);

int main(void) {
    Arena arena = arena_create(1 << 20);

    List eqns = list_create(&arena, sizeof(Equation), 0);
    parse(&eqns, "2024/input/07.txt", &arena);

    long part1 = 0;
    long part2 = 0;
    list_for_each(item, &eqns) {
        Equation *eqn = item->data;
        part1 += solvable(eqn, eqn->test, eqn->length - 1, 0) ? eqn->test : 0;
        part2 += solvable(eqn, eqn->test, eqn->length - 1, 1) ? eqn->test : 0;
    }
    printf("%ld\n", part1);
    printf("%ld\n", part2);

    arena_destroy(&arena);
}

void parse(List *eqns, const char *fname, Arena *arena) {
    FILE *file = fopen(fname, "r");
    assert(file);
    char line[256];
    while (fgets(line, sizeof(line), file)) {
        Equation eqn = {0};
        char *token = strtok(line, " ");
        eqn.test = strtol(token, 0, 10);
        token = strtok(0, " ");
        while (token) {
            eqn.number = realloc(arena, eqn.number, ++eqn.length);
            eqn.number[eqn.length - 1] = strtol(token, 0, 10);
            token = strtok(0, " ");
        }
        list_append(eqns, &eqn);
    }
    fclose(file);
}

long solvable(const Equation *eqn, long res, long i, int part2) {
    if (i == 0) {
        return res == eqn->number[i] ? eqn->test : 0;
    }
    long x = eqn->number[i];
    long d = 1;
    for (long v = x; v; v /= 10) {
        d *= 10;
    }
    return (res > x && solvable(eqn, res - x, i - 1, part2)) ||
           (res % x == 0 && solvable(eqn, res / x, i - 1, part2)) ||
           (part2 && res % d == x && solvable(eqn, res / d, i - 1, part2));
}
