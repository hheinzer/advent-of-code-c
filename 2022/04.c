/*
 * Day 4: Camp Cleanup
 * (https://adventofcode.com/2022/day/4)
 *
 * Part 1:
 * - check if either of the ranges is contained in the other
 *
 * Part 2:
 * - check if there is any overlap between the ranges
 * - notice that it is easier to check for the opposite
 */
#include "aoc.c"

int main(void)
{
    // read input
    const char **line = 0;
    const size_t n_lines = lines_read(&line, "04.txt");

    // count how often one range is fully contained in the other
    long count1 = 0;
    long count2 = 0;
    for (size_t i = 0; i < n_lines; ++i) {
        long a0, a1, b0, b1;
        sscanf(line[i], "%ld-%ld,%ld-%ld", &a0, &a1, &b0, &b1);

        // check whether b is contained in a or otherwise
        if (((a0 <= b0) && (b1 <= a1)) || ((b0 <= a0) && (a1 <= b1))) {
            ++count1;
        }

        // check for no overlap and invert result
        if (!((a1 < b0) || (b1 < a0))) {
            ++count2;
        }
    }

    // part 1
    printf("%ld\n", count1);

    // part 2
    printf("%ld\n", count2);

    // cleanup
    lines_free(line, n_lines);
}
