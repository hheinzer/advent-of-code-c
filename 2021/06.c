/*
 * Day 6: Lanternfish
 * (https://adventofcode.com/2021/day/6)
 *
 * Part 1:
 * - instead of keeping track of each individual fish, keep track of how many fishes
 *   there are of a certain age
 * - simulate 80 reproduction cycles
 *
 * Part 2:
 * - simulate an additional (256 - 80) reproduction cycles
 */
#include "aoc.h"

size_t reproduce(size_t n_fish_of_age[9], size_t n)
{
    for (size_t i = 0; i < n; ++i) {
        const size_t n_new_fish = n_fish_of_age[0];
        for (size_t j = 0; j < 8; ++j) {
            n_fish_of_age[j] = n_fish_of_age[j + 1];
        }
        n_fish_of_age[6] += n_new_fish;
        n_fish_of_age[8] = n_new_fish;
    }
    size_t n_fish = 0;
    for (size_t i = 0; i < 9; ++i) {
        n_fish += n_fish_of_age[i];
    }
    return n_fish;
}

int main(void)
{
    // read input
    const char **line = 0;
    const size_t n_lines = lines_read(&line, "2021/input/06.txt");

    // initialize fish count
    size_t n_fish_of_age[9] = { 0 };
    char *input = strdup(line[0]);
    const char *tok = strtok(input, ",");
    while (tok) {
        size_t age = -1;
        sscanf(tok, "%zu", &age);
        ++n_fish_of_age[age];
        tok = strtok(0, ",");
    }

    // part 1
    printf("%zu\n", reproduce(n_fish_of_age, 80));

    // part 2
    printf("%zu\n", reproduce(n_fish_of_age, 256 - 80));

    // cleanup
    lines_free(line, n_lines);
    free(input);
}
