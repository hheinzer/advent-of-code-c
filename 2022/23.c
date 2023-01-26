/*
 * Day 23: Unstable Diffusion
 * (https://adventofcode.com/2022/day/23)
 *
 * Part 1:
 * - use dict to keep track of the current and proposed positions
 * - follow rules of the simulation and compute number of empty spots at end
 *
 * Part 2:
 * - keep track of how many elves moved
 */
#include "aoc.h"

typedef struct Elf {
    long i;
    long j;
    long prop_i;
    long prop_j;
} Elf;

size_t elf_simulate(List *elf, size_t n_round)
{
    size_t round = 0;
    char key[256] = "";
    Dict *pos = dict_alloc(0, 2 * elf->len);
    for (round = 0; round < n_round; ++round) {
        // log current positions
        for (const Node *node = elf->first; node; node = node->next) {
            const Elf *e = node->data;
            dict_insert(pos, KEY(key, "%ld,%ld", e->i, e->j), 0);
        }

        // propose moves
        Dict *prop = dict_alloc(sizeof(size_t), 2 * elf->len);
        for (const Node *node = elf->first; node; node = node->next) {
            Elf *e = node->data;

            // set default proposition
            e->prop_i = e->i;
            e->prop_j = e->j;

            // check neighbors
            const int N = (dict_find(pos, KEY(key, "%ld,%ld", e->i - 1, e->j)) ? 1 : 0);
            const int S = (dict_find(pos, KEY(key, "%ld,%ld", e->i + 1, e->j)) ? 1 : 0);
            const int W = (dict_find(pos, KEY(key, "%ld,%ld", e->i, e->j - 1)) ? 1 : 0);
            const int E = (dict_find(pos, KEY(key, "%ld,%ld", e->i, e->j + 1)) ? 1 : 0);
            const int NW = (dict_find(pos, KEY(key, "%ld,%ld", e->i - 1, e->j - 1)) ? 1 : 0);
            const int NE = (dict_find(pos, KEY(key, "%ld,%ld", e->i - 1, e->j + 1)) ? 1 : 0);
            const int SW = (dict_find(pos, KEY(key, "%ld,%ld", e->i + 1, e->j - 1)) ? 1 : 0);
            const int SE = (dict_find(pos, KEY(key, "%ld,%ld", e->i + 1, e->j + 1)) ? 1 : 0);

            // check for any neighbor
            if ((N + S + W + E + NE + NW + SE + SW) == 0) {
                continue;
            }

            // propose move
            for (size_t d = 0; d < 4; ++d) {
                if (((round + d) % 4 == 0) && (N + NE + NW == 0)) {
                    e->prop_i = e->i - 1;
                    e->prop_j = e->j;
                    break;

                } else if (((round + d) % 4 == 1) && (S + SE + SW == 0)) {
                    e->prop_i = e->i + 1;
                    e->prop_j = e->j;
                    break;

                } else if (((round + d) % 4 == 2) && (W + NW + SW == 0)) {
                    e->prop_i = e->i;
                    e->prop_j = e->j - 1;
                    break;

                } else if (((round + d) % 4 == 3) && (E + NE + SE == 0)) {
                    e->prop_i = e->i;
                    e->prop_j = e->j + 1;
                    break;
                }
            }

            // log proposed position
            if ((e->i != e->prop_i) || (e->j != e->prop_j)) {
                Item *item = dict_find(prop, KEY(key, "%ld,%ld", e->prop_i, e->prop_j));
                if (item) {
                    size_t *count = item->data;
                    ++(*count);
                    dict_insert(prop, key, count);
                } else {
                    size_t count = 1;
                    dict_insert(prop, key, COPY(count));
                }
            }
        }

        // move elves
        int no_one_moved = 1;
        for (const Node *node = elf->first; node; node = node->next) {
            Elf *e = node->data;
            dict_remove(pos, KEY(key, "%ld,%ld", e->i, e->j));
            if ((e->i != e->prop_i) || (e->j != e->prop_j)) {
                const Item *item = dict_find(prop, KEY(key, "%ld,%ld", e->prop_i, e->prop_j));
                const size_t count = *(size_t *)item->data;
                if (count == 1) {
                    e->i = e->prop_i;
                    e->j = e->prop_j;
                    no_one_moved = 0;
                }
            }
        }

        // cleanup
        dict_free(&prop, free);

        if (no_one_moved) {
            break;
        }
    }

    size_t ret = 0;
    if (round == n_round) { // count empty spots
        long min_i = LONG_MAX;
        long min_j = LONG_MAX;
        long max_i = LONG_MIN;
        long max_j = LONG_MIN;
        for (const Node *node = elf->first; node; node = node->next) {
            const Elf *e = node->data;
            min_i = MIN(min_i, e->i);
            min_j = MIN(min_j, e->j);
            max_i = MAX(max_i, e->i);
            max_j = MAX(max_j, e->j);
        }
        ret = (max_i - min_i + 1) * (max_j - min_j + 1) - elf->len;
    } else {
        ret = round + 1;
    }

    // cleanup
    dict_free(&pos, 0);
    list_free(&elf, free);

    return ret;
}

int main(void)
{
    // read input
    const char **line = 0;
    const size_t n_lines = lines_read(&line, "2022/input/23.txt");

    // create elf locations
    List *elf = list_alloc(sizeof(Elf));
    for (size_t i = 0; i < n_lines; ++i) {
        for (size_t j = 0; j < strlen(line[i]); ++j) {
            if (line[i][j] == '#') {
                Elf e = { i, j, i, j };
                list_insert_last(elf, COPY(e));
            }
        }
    }

    // part 1
    printf("%zu\n", elf_simulate(list_copy(elf, memcpy), 10));

    // part 2
    printf("%zu\n", elf_simulate(list_copy(elf, memcpy), 2000));

    // cleanup
    lines_free(line, n_lines);
    list_free(&elf, free);
}
