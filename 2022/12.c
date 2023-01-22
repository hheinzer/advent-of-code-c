/*
 * Day 12: Hill Climbing Algorithm
 * (https://adventofcode.com/2022/day/12)
 *
 * Part 1:
 * - use breadth first search to find shortest path
 *
 * Part 2:
 * - instead of searching for all 'a' and search from these, we start from all 'b'
 *   - there are a lot of disconnected 'a' and only a line of 'b' at the left side of the
 *   map, which are all connected to an 'a'
 *   - to get the distance from an 'a' we just add 1
 */
#include "aoc.h"

MINMAX(size_t)

size_t shortest_path_length(const char *height, size_t ni, size_t nj, size_t S, size_t E)
{
    // prepare breadth first search (BFS)
    Queue *queue = queue_alloc(sizeof(size_t));
    int *visited = calloc(ni * nj, sizeof(*visited));
    size_t *dist = calloc(ni * nj, sizeof(*dist));

    // initialize BFS
    visited[S] = 1;
    dist[S] = 0;
    queue_push(queue, COPY(S));

    // start BFS
    size_t length = 0;
    const size_t offset[] = { -1, +1, -nj, +nj }; // left, right, up, down
    while (queue->len) {
        // pop front
        size_t *first = queue_pop(queue);
        const size_t f = *first;
        const size_t fi = f / nj;
        const size_t fj = f % nj;
        free(first);

        // check left, right, up, down
        const int check[] = { (fj > 0), (fj < nj - 1), (fi > 0), (fi < ni - 1) };
        for (size_t i = 0; i < 4; ++i) {
            if (check[i]) {
                const size_t n = f + offset[i];
                if (!visited[n] && (height[n] <= height[f] + 1)) {
                    if (n == E) {
                        length = dist[f] + 1;
                        goto cleanup;
                    }
                    visited[n] = 1;
                    dist[n] = dist[f] + 1;
                    queue_push(queue, COPY(n));
                }
            }
        }
    }

cleanup:
    queue_free(&queue, free);
    free(visited);
    free(dist);

    return length;
}

int main(void)
{
    // read input
    const char **line = 0;
    const size_t n_lines = lines_read(&line, "2022/input/12.txt");

    // build height profile
    const size_t ni = n_lines;
    const size_t nj = strlen(line[0]);
    char *height = calloc(ni * nj + 1, sizeof(*height));
    for (size_t i = 0; i < ni; ++i) {
        memcpy(&height[i * nj], line[i], nj * sizeof(*height));
    }

    // find start and end
    const size_t S = strchr(height, 'S') - height;
    const size_t E = strchr(height, 'E') - height;

    // set height of start and end
    height[S] = 'a';
    height[E] = 'z';

    // part 1
    const size_t length = shortest_path_length(height, ni, nj, S, E);
    printf("%zu\n", length);

    // part 2
    size_t min_length = length;
    char *a = strchr(height, 'b');
    while (a) {
        const size_t A = a - height;
        const size_t l = shortest_path_length(height, ni, nj, A, E);
        if (l) {
            min_length = min_size_t(min_length, l + 1);
        }
        a = strchr(a + 1, 'b');
    }
    printf("%zu\n", min_length);

    // cleanup
    lines_free(line, n_lines);
    free(height);
}
