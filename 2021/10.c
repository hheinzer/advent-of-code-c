/*
 * Day 10: Syntax Scoring
 * (https://adventofcode.com/2021/day/10)
 *
 * Part 1:
 * - use a stack to keep track of all openend "([{<" in order
 * - pop the top, when ")]}>" is encountered and check if it matches
 *
 * Part 2:
 * - more of the same, but now add up the remaining stack, if valid
 */
#include "aoc.h"

CMP(size_t)

static const size_t table1[] = {
    [')'] = 3,
    [']'] = 57,
    ['}'] = 1197,
    ['>'] = 25137,
};

static const size_t table2[] = {
    ['('] = 1,
    ['['] = 2,
    ['{'] = 3,
    ['<'] = 4,
};

static const char closing[] = {
    ['('] = ')',
    ['['] = ']',
    ['{'] = '}',
    ['<'] = '>',
};

int is_corrupted(const char *line)
{
    int stack[256] = { 0 };
    size_t n = 0;
    while (*line) {
        if (strchr("([{<", *line)) {
            stack[n++] = *line;
        } else if (closing[stack[--n]] != *line) {
            return *line;
        }
        ++line;
    }
    return 0;
}

size_t complete_if_not_corrupted(const char *line)
{
    int stack[256] = { 0 };
    size_t n = 0;
    while (*line) {
        if (strchr("([{<", *line)) {
            stack[n++] = *line;
        } else if (closing[stack[--n]] != *line) {
            return 0;
        }
        ++line;
    }
    size_t score = 0;
    for (size_t i = n - 1; i < n; --i) {
        score *= 5;
        score += table2[stack[i]];
    }
    return score;
}

int main(void)
{
    // read input
    const char **line = 0;
    const size_t n_lines = lines_read(&line, "2021/input/10.txt");

    // compute syntax error score
    int score = 0;
    for (size_t i = 0; i < n_lines; ++i) {
        score += table1[is_corrupted(line[i])];
    }

    // part 1
    printf("%d\n", score);

    // compute scores incomplete lines
    size_t *scores = calloc(n_lines, sizeof(*scores));
    size_t n_valid = 0;
    for (size_t i = 0; i < n_lines; ++i) {
        if (!is_corrupted(line[i])) {
            scores[n_valid++] = complete_if_not_corrupted(line[i]);
        }
    }
    qsort(scores, n_valid, sizeof(*scores), cmp_size_t_asc);

    // part 2
    printf("%zu\n", scores[n_valid / 2]);

    // cleanup
    lines_free(line, n_lines);
    free(scores);
}
