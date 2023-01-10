/*
 * Day 2: Rock Paper Scissors
 * (https://adventofcode.com/2022/day/2)
 *
 * Part 1:
 * - score for selected shape: 1:Rock(X), 2:Paper(Y), 3:Scissors(Z)
 * - score for outcome: 0:loss, 3:draw, 6:win
 * - figure out if it is a loss, a draw, or a win
 *
 * Part 2:
 * - score for outcome: 0:loss(X), 3:draw(Y), 6:win(Z)
 * - score for selected shape: 1:Rock, 2:Paper, 3:Scissors
 * - figure out if I need to select rock, paper, or scissors
 */
#include "../aoc.h"

int main(void)
{
    // read input
    const char **line = 0;
    const size_t n_lines = lines_read(&line, "02.txt");

    // define points map for first rule set
    const long map1['Z' + 1]['Z' + 1] = {
        ['A'] = { ['X'] = 1 + 3, ['Y'] = 2 + 6, ['Z'] = 3 + 0 },
        ['B'] = { ['X'] = 1 + 0, ['Y'] = 2 + 3, ['Z'] = 3 + 6 },
        ['C'] = { ['X'] = 1 + 6, ['Y'] = 2 + 0, ['Z'] = 3 + 3 },
    };

    // define points map for second rule set
    const long map2['Z' + 1]['Z' + 1] = {
        ['A'] = { ['X'] = 0 + 3, ['Y'] = 3 + 1, ['Z'] = 6 + 2 },
        ['B'] = { ['X'] = 0 + 1, ['Y'] = 3 + 2, ['Z'] = 6 + 3 },
        ['C'] = { ['X'] = 0 + 2, ['Y'] = 3 + 3, ['Z'] = 6 + 1 },
    };

    // compute scores
    long score1 = 0;
    long score2 = 0;
    for (size_t i = 0; i < n_lines; ++i) {
        unsigned char a, b;
        sscanf(line[i], "%c %c", &a, &b);
        score1 += map1[a][b];
        score2 += map2[a][b];
    }

    // part 1
    printf("%ld\n", score1);

    // part 2
    printf("%ld\n", score2);

    // cleanup
    lines_free(line, n_lines);
}
