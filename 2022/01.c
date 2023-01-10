/*
 * Day 1: Calorie Counting
 * (https://adventofcode.com/2022/day/1)
 *
 * Part 1:
 * - add up calories that each elf carries
 * - sort calories in descending order
 * - the elf that carries the most calories is the first one
 *
 * Part 2:
 * - the 3 elves with the most calories are the first 3 in the sorted list
 */
#include "../aoc/aoc.h"

CMP(long)

int main(void)
{
    // read input
    const char **line = 0;
    const size_t n_lines = lines_read(&line, "01.txt");

    // add up calories of every elf, append to cals
    long *cals = calloc(1, sizeof(*cals));
    size_t n_cals = 1;
    for (size_t i = 0; i < n_lines; ++i) {
        if (strlen(line[i]) > 0) {
            long cal = 0;
            sscanf(line[i], "%ld", &cal);
            cals[n_cals - 1] += cal;
        } else {
            cals = realloc(cals, ++n_cals * sizeof(*cals));
            cals[n_cals - 1] = 0;
        }
    }

    // sort calories
    qsort(cals, n_cals, sizeof(*cals), cmp_long_dsc);

    // part 1
    printf("%ld\n", cals[0]);

    // part 2
    printf("%ld\n", cals[0] + cals[1] + cals[2]);

    // cleanup
    lines_free(line, n_lines);
    free(cals);
}
