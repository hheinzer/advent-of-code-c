#include "aoc.h"

typedef enum { adv, bxl, bst, jnz, bxc, out, bdv, cdv } Opcode;

char *parse(long *a, const char *fname, Arena *arena);
char *run(long a, char *ret, const char *prg);
long find(long a, char *ret, const char *prg, long depth);

int main(void) {
    Arena arena = arena_create(mega_byte);

    long a;
    char *prg = parse(&a, "2024/input/17.txt", &arena);

    char *ret = calloc(&arena, ret, 256);
    printf("%s\n", run(a, ret, prg));

    printf("%ld\n", find(0, ret, prg, 0));

    arena_destroy(&arena);
}

char *parse(long *a, const char *fname, Arena *arena) {
    FILE *file = fopen(fname, "r");
    assert(file);
    fscanf(file, "Register A: %ld\n", a);
    char line[256];
    for (long i = 0; i < 4; i++) {
        fgets(line, sizeof(line), file);
    }
    char *str = strchr(line, ' ') + 1;
    *strchr(str, '\n') = 0;
    char *prg = strdup(arena, str);
    fclose(file);
    return prg;
}

long combo(Opcode op, long a, long b, long c) {
    switch (op) {
        case 4:
            return a;
        case 5:
            return b;
        case 6:
            return c;
        case 7:
            abort();
        default:
            return op;
    }
}

char *run(long a, char *ret, const char *prg) {
    long b = 0;
    long c = 0;
    ret[0] = 0;
    for (const char *p = prg, *end = prg + strlen(prg); p < end; p += 4) {
        Opcode code = p[0] - '0';
        Opcode oper = p[2] - '0';
        switch (code) {
            case adv:
                a >>= combo(oper, a, b, c);
                break;
            case bxl:
                b ^= oper;
                break;
            case bst:
                b = combo(oper, a, b, c) % 8;
                break;
            case jnz:
                if (a) {
                    p = prg + (long)(2 * oper) - 4;
                }
                break;
            case bxc:
                b ^= c;
                break;
            case out:
                strcat(ret, (char[]){'0' + (combo(oper, a, b, c) % 8), ',', 0});
                break;
            case bdv:
                b = a >> combo(oper, a, b, c);
                break;
            case cdv:
                c = a >> combo(oper, a, b, c);
                break;
        }
    }
    ret[strlen(ret) - 1] = 0;
    return ret;
}

long find(long a, char *ret, const char *prg, long depth) {
    long prglen = strlen(prg);
    if (2 * depth >= prglen) {
        return a;
    }
    for (long i = 0; i < 8; i++) {
        long aa = (8 * a) + i;
        if (run(aa, ret, prg)[0] == prg[prglen - (2 * depth) - 1]) {
            long res = find(aa, ret, prg, depth + 1);
            if (res) {
                return res;
            }
        }
    }
    return 0;
}
