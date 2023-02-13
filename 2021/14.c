/*
 * Day 14: Extended Polymerization
 * (https://adventofcode.com/2021/day/14)
 *
 * Part 1:
 * - instead of keeping track of the actual polymer string,
 *   keep track of how many of the input pairs there are (left side of rules)
 *   - constant size array, not memory bound
 * - modify polymer by applying the rules in a slightly different way than explained
 *   - example: "CH -> B" uses up "CH" and creates "CB" and "BH"
 * - count the occurrence of individual letters, each one will appear twice, because the
 *   pairs overlap (except for the first and last letter of the initial polymer)
 * - count letters, compute min/max, compute difference
 *
 * Part 2:
 * - repeat with more steps
 */
#include "aoc.h"

void polymer_modify(size_t ni, size_t count[ni], size_t rule[ni][2], size_t n_steps)
{
    size_t *new_count = malloc(ni * sizeof(*new_count));
    for (size_t step = 0; step < n_steps; ++step) {
        memset(new_count, 0, ni * sizeof(*new_count));
        for (size_t i = 0; i < ni; ++i) {
            new_count[rule[i][0]] += count[i];
            new_count[rule[i][1]] += count[i];
        }
        memcpy(count, new_count, ni * sizeof(new_count));
    }
    free(new_count);
}

size_t polymer_quantity_diff(size_t ni, char input[ni][3], size_t count[ni], const char *poly)
{
    // compute histogram of pairs
    size_t hist['Z' - 'A' + 1] = { 0 };
    for (size_t i = 0; i < ni; ++i) {
        hist[input[i][0] - 'A'] += count[i];
        hist[input[i][1] - 'A'] += count[i];
    }

    // add first and last letter of initial polynomial,
    // because they only appear once in the pairs
    ++hist[poly[0] - 'A'];
    ++hist[poly[strlen(poly) - 1] - 'A'];

    // compute min/max of individual letters,
    // each letter is counted twice: divide count by 2
    size_t min = SIZE_MAX;
    size_t max = 0;
    for (char letter = 'A'; letter <= 'Z'; ++letter) {
        hist[letter - 'A'] = hist[letter - 'A'] / 2;
        if (hist[letter - 'A'] > 0) {
            min = MIN(min, hist[letter - 'A']);
        }
        max = MAX(max, hist[letter - 'A']);
    }
    return max - min;
}

int main(void)
{
    // read input
    const char **line = 0;
    const size_t n_lines = lines_read(&line, "2021/input/14.txt");

    // read input pairs and create index map
    const size_t ni = n_lines - 2;
    char(*input)[3] = calloc(ni, sizeof(*input));
    Dict *imap = dict_alloc(sizeof(size_t), 2 * ni);
    for (size_t i = 0; i < ni; ++i) {
        sscanf(line[2 + i], "%s", input[i]);
        dict_insert(imap, input[i], COPY(i));
    }

    // read rules
    size_t(*rule)[2] = calloc(ni, sizeof(*rule));
    for (size_t i = 0; i < ni; ++i) {
        char insert;
        sscanf(line[2 + i], "%*s -> %c", &insert);
        char output0[3] = { input[i][0], insert };
        char output1[3] = { insert, input[i][1] };
        rule[i][0] = *(size_t *)dict_find(imap, output0)->data;
        rule[i][1] = *(size_t *)dict_find(imap, output1)->data;
    }

    // create initial count
    size_t *count = calloc(ni, sizeof(*count));
    for (size_t j = 0; j < strlen(line[0]) - 1; ++j) {
        char pair[3] = { line[0][j], line[0][j + 1] };
        ++count[*(size_t *)dict_find(imap, pair)->data];
    }

    // part 1
    polymer_modify(ni, count, rule, 10);
    printf("%zu\n", polymer_quantity_diff(ni, input, count, line[0]));

    // part 2
    polymer_modify(ni, count, rule, 30);
    printf("%zu\n", polymer_quantity_diff(ni, input, count, line[0]));

    // cleanup
    lines_free(line, n_lines);
    free(input);
    dict_free(&imap, free);
    free(rule);
    free(count);
}
