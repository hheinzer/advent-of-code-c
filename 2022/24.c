/*
 * Day 24: Blizzard Basin
 * (https://adventofcode.com/2022/day/24)
 *
 * Part 1:
 * - use breadth-first-search to find the quickest way to the exit
 * - keep track of with tile have been visited at which time, don't revisit these
 *
 * Part 2:
 * - use bfs multiple times with different start times
 */
#include "aoc.h"

typedef struct State {
    size_t time;
    long i;
    long j;
} State;

size_t bfs(long ni, long nj, char map[ni][nj], const long S[2], const long E[2], size_t t0)
{
    Queue *queue = queue_alloc(sizeof(State));
    Dict *seen = dict_alloc(0, 2 * ni * nj);

    queue_push(queue, COPY(((State) { t0, S[0], S[1] })));

    size_t ret = -1;
    char key[256] = "";
    while (queue->len) {
        State *state = queue_pop(queue);
        const long time = state->time + 1;
        const long i = state->i;
        const long j = state->j;
        free(state);

        static const long di[5] = { -1, +1, +0, +0, +0 };
        static const long dj[5] = { +0, +0, -1, +1, +0 };
        for (size_t d1 = 0; d1 < 5; ++d1) {
            const long ii = i + di[d1];
            const long jj = j + dj[d1];

            if ((ii == E[0]) && (jj == E[1])) {
                ret = time;
                goto cleanup; // reached end
            }

            if (((ii < 0) || (ii >= (long)ni) || (jj < 0) || (jj >= (long)nj))
                && !((ii == S[0]) && (jj == S[1]))) {
                continue; // position is not on map
            }

            int collision = 0;
            if (!((ii == S[0]) && (jj == S[1]))) {
                for (size_t d2 = 0; d2 < 4; ++d2) {
                    const size_t ti = (ni + ii - di[d2] * (time % ni)) % ni;
                    const size_t tj = (nj + jj - dj[d2] * (time % nj)) % nj;
                    static const char b[4] = { '^', 'v', '<', '>' };
                    if (map[ti][tj] == b[d2]) {
                        collision = 1;
                        break; // collision with blizzard
                    }
                }
            }
            if (!collision && !dict_find(seen, KEY(key, "%ld,%ld,%ld", time, ii, jj))) {
                dict_insert(seen, key, 0);
                queue_push(queue, COPY(((State) { time, ii, jj })));
            }
        }
    }
cleanup:
    queue_free(&queue, free);
    dict_free(&seen, 0);
    return ret;
}

int main(void)
{
    // read input
    const char **line = 0;
    const size_t n_lines = lines_read(&line, "2022/input/24.txt");

    // create map
    const size_t ni = n_lines - 2;
    const size_t nj = strlen(line[0]) - 2;
    char(*map)[nj] = calloc(ni, sizeof(*map));
    for (size_t i = 0; i < ni; ++i) {
        memcpy(map[i], &line[1 + i][1], sizeof(map[i]));
    }

    // find start and end
    const long S[2] = { -1, 0 };
    const long E[2] = { ni, nj - 1 };

    // part 1
    const size_t t1 = bfs(ni, nj, map, S, E, 0);
    printf("%zu\n", t1);

    // part 2
    const size_t t2 = bfs(ni, nj, map, E, S, t1);
    const size_t t3 = bfs(ni, nj, map, S, E, t2);
    printf("%zu\n", t3);

    // cleanup
    lines_free(line, n_lines);
    free(map);
}
