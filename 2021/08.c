/*
 * Day 8: Seven Segment Search
 * (https://adventofcode.com/2021/day/8)
 *
 * Part 1:
 * - read entries, sort signals/outputs by char value, and signals by string length
 * - scan outputs for 2, 3, 4, and 7 long strings (digits 1, 7, 4, 8)
 *
 * Part 2:
 * - compute how many segments each digit has with each other digit
 * - decode output by check all possible permutations of the signals until a matching one
 *   is found
 * - because of the sorting, the signals are always [1, 7, 4, {2, 3, 5}, {0, 6, 9}, 8],
 *   {...} meaning all possible permutations of
 */
#include "aoc.h"

CMP(char)

static const char digit[10][8] = {
    [1] = "cf",
    [7] = "acf",
    [4] = "bcdf",
    [2] = "acdeg",
    [3] = "acdfg",
    [5] = "abdfg",
    [0] = "abcefg",
    [6] = "abdefg",
    [9] = "abcdfg",
    [8] = "abcdefg",
};

static const size_t digit_len[10] = {
    [1] = 2,
    [7] = 3,
    [4] = 4,
    [2] = 5,
    [3] = 5,
    [5] = 5,
    [0] = 6,
    [6] = 6,
    [9] = 6,
    [8] = 7,
};

typedef struct Entry {
    char signal[10][8];
    char output[4][8];
} Entry;

int cmp_strlen_asc(const void *a, const void *b)
{
    return strlen(a) - strlen(b);
}

size_t count_common_chars(const char *str1, const char *str2)
{
    const size_t n1 = strlen(str1);
    const size_t n2 = strlen(str2);
    size_t n = 0;
    for (size_t i = 0; i < n1; ++i) {
        for (size_t j = 0; j < n2; ++j) {
            if (str1[i] == str2[j]) {
                ++n;
            }
        }
    }
    return n;
}

size_t entry_decode(size_t common[10][10], const Entry *entry)
{
    static const size_t perm1[6][3] = {
        { 2, 3, 5 },
        { 2, 5, 3 },
        { 3, 2, 5 },
        { 3, 5, 2 },
        { 5, 2, 3 },
        { 5, 3, 2 },
    };
    static const size_t perm2[6][3] = {
        { 0, 6, 9 },
        { 0, 9, 6 },
        { 6, 0, 9 },
        { 6, 9, 0 },
        { 9, 0, 6 },
        { 9, 6, 0 },
    };

    // go through all possible permutations of {2, 3, 5} and {0, 6, 9}
    for (size_t i = 0; i < 6; ++i) {
        for (size_t j = 0; j < 6; ++j) {
            // create code
            const int code[10] = {
                1, 7, 4,
                perm1[i][0], perm1[i][1], perm1[i][2],
                perm2[j][0], perm2[j][1], perm2[j][2],
                8
            };

            // check if code is valid
            int valid = 1;
            for (size_t ii = 0; ii < 10; ++ii) {
                for (size_t jj = 0; jj < 10; ++jj) {
                    valid &= (count_common_chars(entry->signal[ii], entry->signal[jj])
                        == common[code[ii]][code[jj]]);
                }
            }

            if (valid) {
                // return decoded value
                char decode[5] = "";
                for (size_t k = 0; k < 4; ++k) {
                    for (size_t l = 0; l < 10; ++l) {
                        if (!strcmp(entry->output[k], entry->signal[l])) {
                            decode[k] = '0' + code[l];
                        }
                    }
                }
                return strtoul(decode, 0, 10);
            }
        }
    }
    assert(!"Houston we have a problem.");
}

int main(void)
{
    // read input
    const char **line = 0;
    const size_t n_lines = lines_read(&line, "2021/input/08.txt");

    // create entries
    Entry *entry = calloc(n_lines, sizeof(*entry));
    for (size_t i = 0; i < n_lines; ++i) {
        char *input = strdup(line[i]);
        const char *tok = strtok(input, "| ");
        for (size_t j = 0; j < 10; ++j) {
            sscanf(tok, "%7s", entry[i].signal[j]);
            qsort(entry[i].signal[j], strlen(tok), sizeof(*tok), cmp_char_asc);
            tok = strtok(0, "| ");
        }
        for (size_t j = 0; j < 4; ++j) {
            sscanf(tok, "%7s", entry[i].output[j]);
            qsort(entry[i].output[j], strlen(tok), sizeof(*tok), cmp_char_asc);
            tok = strtok(0, "| ");
        }
        qsort(entry[i].signal, 10, sizeof(*entry[i].signal), cmp_strlen_asc);
        free(input);
    }

    // count occurances of 1, 4, 7, and 8
    size_t count = 0;
    for (size_t i = 0; i < n_lines; ++i) {
        for (size_t j = 0; j < 4; ++j) {
            const size_t len = strlen(entry[i].output[j]);
            if ((len == digit_len[1]) || (len == digit_len[7])
                || (len == digit_len[4]) || (len == digit_len[8])) {
                ++count;
            }
        }
    }

    // part 1
    printf("%zu\n", count);

    // count common segments between digits
    size_t common[10][10] = { 0 };
    for (size_t i = 0; i < 10; ++i) {
        for (size_t j = 0; j < 10; ++j) {
            common[i][j] = count_common_chars(digit[i], digit[j]);
        }
    }

    // compute sum of decoded outputs
    size_t sum = 0;
    for (size_t i = 0; i < n_lines; ++i) {
        sum += entry_decode(common, &entry[i]);
    }

    // part 2
    printf("%zu\n", sum);

    // cleanup
    lines_free(line, n_lines);
    free(entry);
}
