/*
 * Day 3: Binary Diagnostic
 * (https://adventofcode.com/2021/day/3)
 *
 * Part 1:
 * - use function to find most common bit in all positions of input list numbers
 * - use inverse of function to find least common bit
 * - compute the power consumption
 *
 * Part 2:
 * - eliminate all number in copy of input list, that do not satisfy the bit criterion
 */
#include "aoc.h"

void find_most_common_bit(char pos[256], const List *input, size_t nj)
{
    // count number of zero bits per position
    size_t n_zeros[256] = { 0 };
    for (const Node *node = input->first; node; node = node->next) {
        const char *number = node->data;
        for (size_t j = 0; j < nj; ++j) {
            if (number[j] == '0') {
                ++n_zeros[j];
            }
        }
    }

    // set most common bit per position
    for (size_t j = 0; j < nj; ++j) {
        const size_t n_ones = input->len - n_zeros[j];
        if (n_zeros[j] == n_ones) {
            pos[j] = 'e';
        } else if (n_zeros[j] > n_ones) {
            pos[j] = '0';
        } else {
            pos[j] = '1';
        }
    }
    pos[nj] = 0;
}

void find_least_common_bit(char pos[256], const List *input, size_t nj)
{
    find_most_common_bit(pos, input, nj);
    for (size_t j = 0; j < nj; ++j) {
        if (pos[j] != 'e') {
            pos[j] = (pos[j] == '0' ? '1' : '0');
        }
    }
}

const char *compute_rating(List *input, size_t nj, void (*find_bit)(char[256], const List *, size_t), char keep)
{
    char pos[256] = "";
    while (input->len > 1) {
        for (size_t j = 0; j < nj; ++j) {
            find_bit(pos, input, nj);
            Node *next = 0;
            for (Node *node = input->first; node; node = next) {
                next = node->next;
                const char *number = node->data;
                if (pos[j] != 'e') {
                    if (number[j] != pos[j]) {
                        list_delete(input, node);
                    }
                } else {
                    if (number[j] != keep) {
                        list_delete(input, node);
                    }
                }
            }
            if (input->len == 1) {
                break;
            }
        }
    }
    assert(input->len == 1);

    const char *ret = input->first->data;

    list_free(&input, 0);

    return ret;
}

int main(void)
{
    // read input
    const char **line = 0;
    const size_t n_lines = lines_read(&line, "2021/input/03.txt");

    // create list of line pointers
    List *input = list_alloc(0);
    for (size_t i = 0; i < n_lines; ++i) {
        list_insert_last(input, (void *)line[i]);
    }

    // compute gamma rate
    const size_t nj = strlen(line[0]);
    char gamma[256] = "";
    find_most_common_bit(gamma, input, nj);

    // compute epsilon rate
    char epsilon[256] = "";
    find_least_common_bit(epsilon, input, nj);

    // part 1
    printf("%ld\n", strtol(gamma, 0, 2) * strtol(epsilon, 0, 2));

    // compute oxygen generator rating
    const char *oxygen = compute_rating(list_copy(input, 0), nj, find_most_common_bit, '1');

    // compute CO2 scrubber rating
    const char *scrubber = compute_rating(list_copy(input, 0), nj, find_least_common_bit, '0');

    // part 2
    printf("%ld\n", strtol(oxygen, 0, 2) * strtol(scrubber, 0, 2));

    // cleanup
    lines_free(line, n_lines);
    list_free(&input, 0);
}
