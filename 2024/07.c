#include "aoc.h"

typedef struct {
    long test;
    long *number;
    long length;
} Equation;

typedef long Operation(long, long);

void parse(List *eqns, const char *fname, Arena *arena);
long compute(const List *eqns, Operation **op, long nops, Arena arena);
Operation add, mul, cat;

int main(void) {
    Arena arena = arena_create(1 << 20);

    List eqns = list_create(&arena, sizeof(Equation), 0);
    parse(&eqns, "2024/input/07.txt", &arena);

    printf("%ld\n", compute(&eqns, (Operation *[]){add, mul}, 2, arena));

    printf("%ld\n", compute(&eqns, (Operation *[]){add, mul, cat}, 3, arena));

    arena_destroy(&arena);
}

void parse(List *eqns, const char *fname, Arena *arena) {
    FILE *file = fopen(fname, "r");
    assert(file);
    char line[256];
    while (fgets(line, sizeof(line), file)) {
        Equation eqn = {0};
        const char *token = strtok(line, " ");
        eqn.test = strtol(token, 0, 0);
        token = strtok(0, " ");
        while (token) {
            eqn.number = realloc(arena, eqn.number, ++eqn.length);
            eqn.number[eqn.length - 1] = strtol(token, 0, 0);
            token = strtok(0, " ");
        }
        list_append(eqns, &eqn);
    }
    fclose(file);
}

long compute(const List *eqns, Operation **op, long nops, Arena arena) {
    long total = 0;
    Equation *eqn = list_to_array(eqns, &arena);
#pragma omp parallel for reduction(+ : total) schedule(auto)
    for (long e = 0; e < eqns->length; e++) {
        long repeat = eqn[e].length - 1;
        long ncombs = lpow(nops, repeat);
        for (long comb = 0; comb < ncombs; comb++) {
            long test = eqn[e].number[0];
            for (long i = 0, j = comb; i < repeat; i++, j /= nops) {
                test = op[j % nops](test, eqn[e].number[i + 1]);
            }
            if (test == eqn[e].test) {
                total += test;
                break;
            }
        }
    }
    return total;
}

long add(long a, long b) {
    return a + b;
}

long mul(long a, long b) {
    return a * b;
}

long cat(long a, long b) {
    char tmp[128];
    sprintf(tmp, "%ld%ld", a, b);
    return strtol(tmp, 0, 10);
}
