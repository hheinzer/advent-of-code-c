/*
 * Day 25: Full of Hot Air
 * (https://adventofcode.com/2022/day/25)
 *
 * Part 1:
 * - just like normal base conversion, but with negative numbers allowed
 */
#include "aoc.h"

long snafu_to_decimal(const char *snafu)
{
    static const char *code = "=-012";
    const size_t n = strlen(snafu);
    const char *s = &snafu[n - 1];
    long d = 0;
    long c = 1;
    for (size_t i = 0; i < n; ++i) {
        d += c * (strchr(code, *s) - code - 2);
        c *= 5;
        --s;
    }
    return d;
}

char *string_reverse(char *str)
{
    const size_t n = strlen(str);
    for (size_t i = 0, j = n - 1; i < n / 2; ++i, --j) {
        SWAP(str[i], str[j]);
    }
    return str;
}

char *decimal_to_snafu(long decimal, char *snafu)
{
    static const char *code = "012=-";
    char *s = snafu;
    while (decimal) {
        long r = decimal % 5;
        decimal /= 5;
        *s = code[r];
        if (r > 2) {
            decimal += 1;
        }
        ++s;
    }
    *s = 0;
    return string_reverse(snafu);
}

int main(void)
{
    // read input
    const char **line = 0;
    const size_t n_lines = lines_read(&line, "2022/input/25.txt");

    // compute the sum of the snafu numbers
    long sum = 0;
    char snafu[256] = "";
    for (size_t i = 0; i < n_lines; ++i) {
        sum += snafu_to_decimal(line[i]);
    }

    // part 1
    printf("%s\n", decimal_to_snafu(sum, snafu));

    // cleanup
    lines_free(line, n_lines);
}
