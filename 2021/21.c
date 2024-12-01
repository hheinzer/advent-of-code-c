/*
 * Day 21: Dirac Dice
 * (https://adventofcode.com/2021/day/21)
 *
 * Part 1:
 * - straight forward, play the dice game according to the rules
 *
 * Part 2:
 * - the straight forward way of playing the game with dirac dice does not work
 *   if produces too many states
 *   - reduce states by swapping the function input
 *   - use caching to reduce number of function calls
 */
#include "aoc.h"

size_t play_with_deterministic_die(size_t position[2])
{
    size_t die = 0;
    size_t count = 0;
    size_t score[2] = {0};
    while ((score[0] < 1000) && (score[1] < 1000)) {
        for (size_t i = 0; i < 2; ++i) {
            // roll die
            size_t move = 0;
            for (size_t j = 0; j < 3; ++j) {
                die = die % 100 + 1;
                move += die;
            }
            count += 3;

            // update position and score
            position[i] = (position[i] - 1 + move) % 10 + 1;
            score[i] += position[i];

            // check if player won
            if (score[i] >= 1000) break;
        }
    }
    return MIN(score[0], score[1]) * count;
}

void play_with_dirac_die(size_t p0, size_t p1, size_t s0, size_t s1, size_t *w0_, size_t *w1_,
                         Dict *cache)
{
    // check if result exists in cache
    char key[256] = "";
    Item *item = dict_find(cache, KEY(key, "%zu,%zu,%zu,%zu", p0, p1, s0, s1));
    if (item) {
        const size_t *w = item->data;
        *w0_ = w[0];
        *w1_ = w[1];
        return;
    }

    // compute number of wins
    size_t w0 = 0;
    size_t w1 = 0;
    for (size_t i = 1; i <= 3; ++i) {
        for (size_t j = 1; j <= 3; ++j) {
            for (size_t k = 1; k <= 3; ++k) {
                const size_t np0 = (p0 - 1 + i + j + k) % 10 + 1;
                const size_t ns0 = s0 + np0;
                if (ns0 >= 21) {
                    w0 += 1;
                }
                else {
                    size_t nw0 = 0;
                    size_t nw1 = 0;
                    play_with_dirac_die(p1, np0, s1, ns0, &nw1, &nw0, cache);
                    w0 += nw0;
                    w1 += nw1;
                }
            }
        }
    }
    *w0_ = w0;
    *w1_ = w1;

    // insert result into cache
    size_t data[2] = {w0, w1};
    dict_insert(cache, key, memdup(data, sizeof(data)));
}

int main(void)
{
    // read input
    const char **line = 0;
    const size_t n_lines = lines_read(&line, "2021/input/21.txt");

    // get starting position
    size_t p0 = 0;
    size_t p1 = 0;
    sscanf(line[0], "Player 1 starting position: %zu", &p0);
    sscanf(line[1], "Player 2 starting position: %zu", &p1);

    // part 1
    printf("%zu\n", play_with_deterministic_die((size_t[2]){p0, p1}));

    // part 2
    size_t w0 = 0;
    size_t w1 = 0;
    Dict *cache = dict_alloc(sizeof(size_t[2]), 30000);
    play_with_dirac_die(p0, p1, 0, 0, &w0, &w1, cache);
    printf("%zu\n", MAX(w0, w1));

    // cleanup
    lines_free(line, n_lines);
    dict_free(&cache, free);
}
