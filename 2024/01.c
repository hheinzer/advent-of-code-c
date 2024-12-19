#include "aoc.h"

int main(void)
{
    Array lines = read_lines("2024/input/01.txt");
    long *left = calloc(lines.size, sizeof(*left));
    long *right = calloc(lines.size, sizeof(*right));
    for (long i = 0; i < lines.size; ++i)
        sscanf(lines.item[i].data, "%ld %ld", &left[i], &right[i]);

    // part one
    qsort(left, lines.size, sizeof(*left), longcmp);
    qsort(right, lines.size, sizeof(*right), longcmp);
    long sum1 = 0;
    for (long i = 0; i < lines.size; ++i) sum1 += labs(left[i] - right[i]);
    printf("%ld\n", sum1);

    // part two
    long sum2 = 0;
    for (long i = 0; i < lines.size; ++i)
        for (long j = 0; j < lines.size; ++j)
            if (left[i] == right[j]) sum2 += left[i];
    printf("%ld\n", sum2);
}
