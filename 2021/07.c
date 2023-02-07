/*
 * Day 7: The Treachery of Whales
 * (https://adventofcode.com/2021/day/7)
 *
 * Part 1:
 * - the position that minimizes the cost is the median position
 *
 * Part 2:
 * - use Gauss' formula to compute the cost for a single distance
 * - the position that minimizes the cost should be the mean, but because of integer
 *   rounding it is only in the general vicinity of the mean
 */
#include "aoc.h"

CMP(long)

size_t strcnt(const char *str, const char c)
{
    size_t n = 0;
    while (*str) {
        if (*(str++) == c) {
            ++n;
        }
    }
    return n;
}

long pos_median(long *pos, size_t n)
{
    qsort(pos, n, sizeof(*pos), cmp_long_asc);
    return pos[n / 2];
}

long pos_cost1(long *pos, size_t n, long p)
{
    long cost = 0;
    for (size_t i = 0; i < n; ++i) {
        cost += labs(p - pos[i]);
    }
    return cost;
}

long pos_mean(long *pos, size_t n)
{
    long mean = 0;
    for (size_t i = 0; i < n; ++i) {
        mean += pos[i];
    }
    return round(mean / (double)n);
}

long pos_cost2(long *pos, size_t n, long p, long dp)
{
    long min_cost = LONG_MAX;
    for (long _dp = -dp; _dp <= dp; ++_dp) {
        long cost = 0;
        for (size_t i = 0; i < n; ++i) {
            const long d = labs(p + _dp - pos[i]);
            cost += d * (d + 1) / 2;
        }
        min_cost = MIN(min_cost, cost);
    }
    return min_cost;
}

int main(void)
{
    // read input
    const char **line = 0;
    const size_t n_lines = lines_read(&line, "2021/input/07.txt");

    // read crab submarine positions
    const size_t n = strcnt(line[0], ',') + 1;
    long *pos = calloc(n, sizeof(*pos));
    char *input = STRCOPY(line[0]);
    const char *tok = strtok(input, ",");
    for (size_t i = 0; i < n; ++i) {
        sscanf(tok, "%ld", &pos[i]);
        tok = strtok(0, ",");
    }

    // part 1
    printf("%ld\n", pos_cost1(pos, n, pos_median(pos, n)));

    // part 2
    printf("%ld\n", pos_cost2(pos, n, pos_mean(pos, n), 10));

    // cleanup
    lines_free(line, n_lines);
    free(pos);
    free(input);
}
