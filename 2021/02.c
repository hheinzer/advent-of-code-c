/*
 * Day 2: Dive!
 * (https://adventofcode.com/2021/day/2)
 *
 * Part 1:
 * - follow the instructions to pilot the submarine
 *
 * Part 2:
 * - follow the new instructions to pilot the submarine
 */
#include "aoc.h"

int main(void)
{
    // read input
    const char **line = 0;
    const size_t n_lines = lines_read(&line, "2021/input/02.txt");

    // pilot submarine
    long position = 0;
    long depth = 0;
    for (size_t i = 0; i < n_lines; ++i) {
        long X = 0;
        sscanf(line[i], "%*s %ld", &X);
        switch (line[i][0]) {
        case 'f':
            position += X;
            break;
        case 'd':
            depth += X;
            break;
        case 'u':
            depth -= X;
            break;
        default:
            assert(!"Illegal direction encountered.");
        }
    }

    // part 1
    printf("%ld\n", position * depth);

    // pilot submarine again, now with aim
    position = 0;
    depth = 0;
    long aim = 0;
    for (size_t i = 0; i < n_lines; ++i) {
        long X = 0;
        sscanf(line[i], "%*s %ld", &X);
        switch (line[i][0]) {
        case 'f':
            position += X;
            depth += aim * X;
            break;
        case 'd':
            aim += X;
            break;
        case 'u':
            aim -= X;
            break;
        default:
            assert(!"Illegal direction encountered.");
        }
    }

    // part 2
    printf("%ld\n", position * depth);

    // cleanup
    lines_free(line, n_lines);
}
