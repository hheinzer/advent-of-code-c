/*
 * Day 15: Chiton
 * (https://adventofcode.com/2021/day/15)
 *
 * Part 1:
 * - create grid
 * - use dijkstra with priority queue (heap) to find the shortest path
 *
 * Part 2:
 * - enlarge grid
 * - use dijkstra again
 */
#include "aoc.h"

size_t dijkstra(size_t ni, size_t nj, int grid[ni][nj])
{
    // static offsets to reach neighbors
    static const long di[4] = { -1, +1, +0, +0 };
    static const long dj[4] = { +0, +0, -1, +1 };

    // set up dijkstra search
    Heap *heap = heap_alloc(sizeof(size_t[2]), ni * nj);
    size_t(*dist)[nj] = malloc(ni * sizeof(*dist));
    for (size_t i = 1; i < ni - 1; ++i) {
        for (size_t j = 1; j < nj - 1; ++j) {
            dist[i][j] = SIZE_MAX;
        }
    }
    int(*visited)[nj] = calloc(ni, sizeof(*visited));

    // insert starting tile, set its distance to 0, and mark it as visited
    heap_insert(heap, 0, COPY(((size_t[]) { 1, 1 })));
    dist[1][1] = 0;
    visited[1][1] = 1;

    // dijkstra
    size_t ret = 0;
    while (heap->len > 0) {
        // get the highest priority element (shortest distance)
        size_t *u = heap_remove(heap);
        const size_t ui = u[0];
        const size_t uj = u[1];
        free(u);

        // check if it is the end
        if ((ui == ni - 2) && (uj == nj - 2)) {
            ret = dist[ui][uj];
            goto cleanup;
        }

        // mark u as visited
        visited[ui][uj] = 1;

        // check neighbors
        for (size_t k = 0; k < 4; ++k) {
            const size_t vi = ui + di[k];
            const size_t vj = uj + dj[k];

            // go to valid neighbor, if it has not been visited yet
            if ((grid[vi][vj] > 0) && !visited[vi][vj]) {
                // compute total distance to neighbor
                const size_t alt = dist[ui][uj] + grid[vi][vj];

                // if it is smaller, then update neighbor distance and add neighbor to heap
                if (alt < dist[vi][vj]) {
                    dist[vi][vj] = alt;
                    heap_insert(heap, -alt, COPY(((size_t[]) { vi, vj })));
                }
            }
        }
    }

cleanup:
    heap_free(&heap, free);
    free(dist);
    free(visited);
    return ret;
}

CMP(size_t)

// this is just here for a speed comparison, don't use it,
// it is slower than dijkstra with heap
size_t dijkstra_sorted_list(size_t ni, size_t nj, int grid[ni][nj])
{
    // static offsets to reach neighbors
    static const long di[4] = { -1, +1, +0, +0 };
    static const long dj[4] = { +0, +0, -1, +1 };

    // set up dijkstra search
    Queue *queue = queue_alloc(sizeof(size_t[3]));
    size_t(*dist)[nj] = malloc(ni * sizeof(*dist));
    for (size_t i = 1; i < ni - 1; ++i) {
        for (size_t j = 1; j < nj - 1; ++j) {
            dist[i][j] = SIZE_MAX;
        }
    }
    int(*visited)[nj] = calloc(ni, sizeof(*visited));

    // insert starting tile, set its distance to 0, and mark it as visited
    queue_push(queue, COPY(((size_t[]) { 0, 1, 1 })));
    dist[1][1] = 0;
    visited[1][1] = 1;

    // dijkstra
    size_t ret = 0;
    while (queue->len > 0) {
        // get the highest priority element (shortest distance)
        list_sort(queue, cmp_size_t_asc);
        size_t *u = queue_pop(queue);
        const size_t ui = u[1];
        const size_t uj = u[2];
        free(u);

        // check if it is the end
        if ((ui == ni - 2) && (uj == nj - 2)) {
            ret = dist[ui][uj];
            goto cleanup;
        }

        // mark u as visited
        visited[ui][uj] = 1;

        // check neighbors
        for (size_t k = 0; k < 4; ++k) {
            const size_t vi = ui + di[k];
            const size_t vj = uj + dj[k];

            // go to valid neighbor, if it has not been visited yet
            if ((grid[vi][vj] > 0) && !visited[vi][vj]) {
                // compute total distance to neighbor
                const size_t alt = dist[ui][uj] + grid[vi][vj];

                // if it is smaller, then update neighbor distance and add neighbor to heap
                if (alt < dist[vi][vj]) {
                    dist[vi][vj] = alt;
                    queue_push(queue, COPY(((size_t[]) { alt, vi, vj })));
                }
            }
        }
    }

cleanup:
    queue_free(&queue, free);
    free(dist);
    free(visited);
    return ret;
}

int main(void)
{
    // read input
    const char **line = 0;
    const size_t n_lines = lines_read(&line, "2021/input/15.txt");

    // create grid with padding
    const size_t ni = n_lines + 2;
    const size_t nj = strlen(line[0]) + 2;
    int(*grid)[nj] = calloc(ni, sizeof(*grid));
    for (size_t i = 0; i < ni - 2; ++i) {
        for (size_t j = 0; j < nj - 2; ++j) {
            grid[1 + i][1 + j] = line[i][j] - '0';
        }
    }

    // part 1
    printf("%zu\n", dijkstra(ni, nj, grid));

    // enlarge grid
    const size_t new_ni = 5 * (ni - 2) + 2;
    const size_t new_nj = 5 * (nj - 2) + 2;
    int(*new_grid)[new_nj] = calloc(new_ni, sizeof(*new_grid));
    for (size_t i = 1, addi = 0; i < new_ni - 1; i += ni - 2, ++addi) {
        for (size_t j = 1, addj = 0; j < new_nj - 1; j += nj - 2, ++addj) {
            for (size_t ii = 0; ii < ni - 2; ++ii) {
                for (size_t jj = 0; jj < nj - 2; ++jj) {
                    new_grid[i + ii][j + jj] = grid[1 + ii][1 + jj] + addi + addj;
                    if (new_grid[i + ii][j + jj] > 9) {
                        new_grid[i + ii][j + jj] -= 9;
                    }
                }
            }
        }
    }

    // part 2
    printf("%zu\n", dijkstra(new_ni, new_nj, new_grid));

    // cleanup
    lines_free(line, n_lines);
    free(grid);
    free(new_grid);
}
