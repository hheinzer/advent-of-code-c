#include "aoc.h"

typedef struct {
    Vec2 a;
    Vec2 b;
    Vec2 prize;
} Claw;

void parse(List *claws, const char *fname);
long mintokens(const Claw *claw, long add);

int main(void) {
    Arena arena = arena_create(1 << 20);

    List claws = list_create(&arena, sizeof(Claw), 0);
    parse(&claws, "2024/input/13.txt");

    long part1 = 0;
    long part2 = 0;
    list_for_each(item, &claws) {
        part1 += mintokens(item->data, 0);
        part2 += mintokens(item->data, 10000000000000);
    }
    printf("%ld\n", part1);
    printf("%ld\n", part2);

    arena_destroy(&arena);
}

void parse(List *claws, const char *fname) {
    FILE *file = fopen(fname, "r");
    assert(file);
    char line[256];
    Claw claw;
    while (fgets(line, sizeof(line), file)) {
        if (sscanf(line, "Button A: X%ld, Y%ld", &claw.a.r, &claw.a.c) == 2) {
        }
        else if (sscanf(line, "Button B: X%ld, Y%ld", &claw.b.r, &claw.b.c) == 2) {
        }
        else if (sscanf(line, "Prize: X=%ld, Y=%ld", &claw.prize.r, &claw.prize.c) == 2) {
        }
        else {
            list_append(claws, &claw);
        }
    }
    list_append(claws, &claw);
    fclose(file);
}

long mintokens(const Claw *claw, long add) {
    long a = claw->a.r;
    long b = claw->b.r;
    long c = claw->a.c;
    long d = claw->b.c;
    long x = claw->prize.r + add;
    long y = claw->prize.c + add;
    long div = a * d - b * c;
    if ((d * x - b * y) % div == 0 && (-c * x + a * y) % div == 0) {
        return (3 * (d * x - b * y) + 1 * (-c * x + a * y)) / div;
    }
    return 0;
}
