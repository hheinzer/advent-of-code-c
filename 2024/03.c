#include "aoc.h"

char *parse(const char *fname, Arena *arena);
long compute(const char *input);
char *filter(char *input);

int main(void) {
    Arena arena = arena_create(mega_byte);

    char *input = string_parse("2024/input/03.txt", "\n", &arena);

    printf("%ld\n", compute(input));
    printf("%ld\n", compute(filter(input)));

    arena_destroy(&arena);
}

long compute(const char *input) {
    long sum = 0;
    while (true) {
        char *begin = strstr(input, "mul(");
        if (!begin || !strstr(begin, ")")) {
            return sum;
        }
        char *c = begin + 4;
        if (isdigit(*c)) {
            while (isdigit(*c)) {
                c++;
            };
            if (*c == ',' && isdigit(*(c + 1))) {
                c += 1;
                while (isdigit(*c)) {
                    c++;
                };
                if (*c == ')') {
                    long x;
                    long y;
                    sscanf(begin, "mul(%ld,%ld)", &x, &y);
                    sum += x * y;
                }
            }
        }
        input = c;
    }
}

char *filter(char *input) {
    char *str = input;
    while (true) {
        char *begin = strstr(str, "don't()");
        if (!begin) {
            return input;
        }
        char *end = strstr(begin, "do()");
        if (!end) {
            return input;
        }
        for (char *c = begin; c < end; c++) {
            *c = '-';
        }
        str = end + 4;
    }
}
