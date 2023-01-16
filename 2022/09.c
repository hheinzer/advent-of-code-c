/*
 * Day 9: Rope Bridge
 * (https://adventofcode.com/2022/day/9)
 *
 * Part 1:
 * - moving the head is straight forward
 * - following can be implemented with sign, ceil, and abs for integers
 * - determining where the tail has been is done with a dict
 *   - every position is converted to a string that is used as the key
 *   - we don't need to add any data, so this is effectively a set
 *   - at the end, count how many elements are in the dict
 *
 * Part 2:
 * - the implementation is generalized, so we can rerun it with 10 knots
 */
#include "aoc.h"

typedef struct Position {
    long x;
    long y;
} Position;

void position_move(Position *pos, char dir)
{
    switch (dir) {
    case 'L':
        --pos->x;
        break;
    case 'R':
        ++pos->x;
        break;
    case 'U':
        ++pos->y;
        break;
    case 'D':
        --pos->y;
        break;
    default:
        assert(!"Illegal direction encountered.");
    }
}

void position_follow(Position *tail, const Position *head)
{
    const long dx = head->x - tail->x;
    const long dy = head->y - tail->y;
    if ((labs(dx) == 2) || labs(dy) == 2) {
        tail->x += SIGN(dx) * CEIL(labs(dx), 2);
        tail->y += SIGN(dy) * CEIL(labs(dy), 2);
    }
}

int main(void)
{
    // read input
    const char **line = 0;
    const size_t n_lines = lines_read(&line, "2022/input/09.txt");

    // simulate rope with 2 and 10 knots
    const size_t nr[] = { 2, 10 };
    char key[256] = "";
    for (size_t r = 0; r < LEN(nr); ++r) {
        // initialize rope
        Position rope[nr[r]];
        memset(rope, 0, sizeof(rope));

        // create dict for tail positions (used as set: data=0)
        Dict *pos = dict_alloc(0, 7500);

        // run simulation
        for (size_t i = 0; i < n_lines; ++i) {
            char dir = 0;
            size_t n = 0;
            sscanf(line[i], "%c %zu", &dir, &n);
            for (size_t j = 0; j < n; ++j) {
                position_move(&rope[0], dir);
                for (size_t k = 1; k < nr[r]; ++k) {
                    position_follow(&rope[k], &rope[k - 1]);
                }

                // insert tail position into dict
                dict_insert(pos,
                    KEY(key, "%ld,%ld", rope[nr[r] - 1].x, rope[nr[r] - 1].y), 0);
            }
        }

        // print result
        printf("%zu\n", pos->len);

        // cleanup
        dict_free(&pos, 0);
    }

    // cleanup
    lines_free(line, n_lines);
}
