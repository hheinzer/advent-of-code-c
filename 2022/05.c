/*
 * Day 5: Supply Stacks
 * (https://adventofcode.com/2022/day/5)
 *
 * Part 1:
 * - represent crate towers as stacks
 *
 * Part 2:
 * - use intermediate stack to invert crate order
 */
#include "../aoc/aoc.h"

void solve(const char **line, size_t n_lines, size_t cratemover)
{
    // find separator line
    const size_t i0 = line_find(line, n_lines, "");

    // create crate stacks
    List *stack[9] = { 0 };
    size_t n_stacks = 0;
    for (size_t i = 0; i < i0; ++i) {
        const size_t n = strlen(line[i]);
        for (size_t j = 1; j < n; j += 4) {
            char crate = line[i][j];
            if (('A' <= crate) && (crate <= 'Z')) {
                // compute the stack index
                const size_t istack = (j - 1) / 4;

                // increase number of stack if necessary
                for (size_t k = n_stacks; k < istack + 1; ++k) {
                    stack[n_stacks++] = list_alloc(sizeof(char));
                }

                // add crate
                list_insert_front(stack[istack], COPY(crate));
            }
        }
    }

    // follow movement instructions
    List *tmp = list_alloc(sizeof(char));
    for (size_t iline = i0 + 1; iline < n_lines; ++iline) {
        size_t n = 0, f = 0, t = 0;
        sscanf(line[iline], "move %zu from %zu to %zu", &n, &f, &t);
        switch (cratemover) {
        case 9000:
            for (size_t i = 0; i < n; ++i) {
                list_insert_back(stack[t - 1], list_remove_back(stack[f - 1]));
            }
            break;
        case 9001:
            for (size_t i = 0; i < n; ++i) {
                list_insert_back(tmp, list_remove_back(stack[f - 1]));
            }
            for (size_t i = 0; i < n; ++i) {
                list_insert_back(stack[t - 1], list_remove_back(tmp));
            }
            break;
        default:
            assert(!"Illegal cratemover encountered.");
        }
    }
    list_free(&tmp, free);

    // print topmost of every stack
    for (size_t i = 0; i < n_stacks; ++i) {
        printf("%c", *(char *)stack[i]->tail->data);
    }
    printf("\n");

    // cleanup
    for (size_t i = 0; i < n_stacks; ++i) {
        list_free(&stack[i], free);
    }
}

int main(void)
{
    // read input
    const char **line = 0;
    const size_t n_lines = lines_read(&line, "05.txt");

    // part 1
    solve(line, n_lines, 9000);

    // part 2
    solve(line, n_lines, 9001);

    // cleanup
    lines_free(line, n_lines);
}
