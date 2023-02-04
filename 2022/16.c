/*
 * Day 16: Proboscidea Volcanium
 * (https://adventofcode.com/2022/day/16)
 *
 * Part 1:
 * - find all the non-zero flow rate valves, only they are interesting
 * - compute the cost (distances to + 1 minute open valve) from any valve to any other
 *   valve
 * - search all possible ways of visiting the valves and find the max flow rate,
 *   for 1 player, (DFS with state caching, memoization)
 *
 * Part 2:
 * - repeat with 2 players and less time
 */
#include "aoc.h"

size_t bfs(size_t n, const size_t *_adj, size_t S, size_t E)
{
    // breadth first search
    Queue *q = queue_alloc(sizeof(size_t));
    int *visited = calloc(n, sizeof(*visited));
    size_t *distance = calloc(n, sizeof(*distance));
    visited[S] = 1;
    distance[S] = 0;
    queue_push(q, COPY(S));
    size_t min_distance = 0;
    const size_t(*adj)[n] = TENSOR(adj, _adj);
    while (q->len > 0) {
        void *data = queue_pop(q);
        size_t i = *(size_t *)data;
        free(data);
        for (size_t j = 0; j < n; ++j) {
            if (adj[i][j] && !visited[j]) {
                if (j == E) {
                    min_distance = distance[i] + 1;
                    goto cleanup;
                }
                visited[j] = 1;
                distance[j] = distance[i] + 1;
                queue_push(q, COPY(j));
            }
        }
    }
cleanup:
    queue_free(&q, free);
    free(visited);
    free(distance);
    return min_distance;
}

size_t _dfs(Dict *cache, size_t n, const size_t *rate, const size_t *_cost, char *seen,
    size_t loc, size_t AA, size_t time_allowed, size_t time_taken, size_t n_players)
{
    if (time_taken == time_allowed) {
        if (n_players > 1) {
            return _dfs(cache, n, rate, _cost, seen, AA, AA, time_allowed, 0, n_players - 1);
        } else {
            return 0;
        }
    }

    // check if state has been evaluated before
    char key[256] = "";
    const size_t time_remaining = time_allowed - time_taken;
    snprintf(key, sizeof(key), "%zu %zu %s %zu", loc, time_remaining, seen, n_players);
    Item *item = dict_find(cache, key);
    if (item) {
        return *(size_t *)item->data;
    }

    // mark valve at location as seen
    char reset = seen[loc];
    seen[loc] = '1';

    // check all possible options
    size_t flow = 0;
    const size_t(*cost)[n] = TENSOR(cost, _cost);
    for (size_t i = 0; i < n; ++i) {
        // go to unseen valve if there is enough time
        if ((seen[i] == '0') && (cost[loc][i] < time_remaining)) {
            size_t _flow = (time_remaining - cost[loc][i]) * rate[i]
                + _dfs(cache, n, rate, _cost, seen, i,
                    AA, time_allowed, time_taken + cost[loc][i], n_players);
            flow = MAX(flow, _flow);
        }
    }

    // stop opening valves
    flow = MAX(flow,
        _dfs(cache, n, rate, _cost, seen, loc, AA, time_allowed, time_allowed, n_players));

    // reset valve at location
    seen[loc] = reset;

    // add state to cache
    dict_insert(cache, key, COPY(flow));

    return flow;
}

size_t dfs(size_t n, const size_t *rate, const List *non_zero, const size_t *cost,
    size_t loc, size_t time_allowed, size_t n_players)
{
    // create array that marks closed, non-zero valves as '0'
    char seen[n + 1];
    memset(seen, '1', sizeof(seen));
    seen[n] = 0;
    for (const Node *node = non_zero->first; node; node = node->next) {
        seen[*(size_t *)node->data] = '0';
    }
    Dict *cache = dict_alloc(sizeof(size_t), 3000000);
    size_t ret = _dfs(cache, n, rate, cost, seen, loc, loc, time_allowed, 0, n_players);
    dict_free(&cache, free);
    return ret;
}

int main(void)
{
    // read input
    const char **line = 0;
    const size_t n_lines = lines_read(&line, "2022/input/16.txt");

    // create map between valve name and index, read flow rates, find non-zero valves
    Dict *v2i = dict_alloc(sizeof(size_t), 2 * n_lines);
    size_t *rate = calloc(n_lines, sizeof(*rate));
    List *non_zero = list_alloc(sizeof(size_t));
    for (size_t i = 0; i < n_lines; ++i) {
        char key[3] = "";
        sscanf(line[i], "Valve %s has flow rate=%zu", key, &rate[i]);
        dict_insert(v2i, key, COPY(i));
        if (rate[i] > 0) {
            list_insert_last(non_zero, COPY(i));
        }
    }
    const size_t AA = *(size_t *)dict_find(v2i, "AA")->data;

    // create network
    size_t(*adj)[n_lines] = calloc(n_lines, sizeof(*adj));
    for (size_t i = 0; i < n_lines; ++i) {
        char key[3] = "";
        char net[256] = "";
        sscanf(line[i], "Valve %s has flow rate=%*d; %*s %*s to %*s %[^\n]", key, net);
        size_t iv = *(size_t *)dict_find(v2i, key)->data;
        char *tok = strtok(net, ",");
        while (tok) {
            sscanf(tok, " %s", key);
            size_t jv = *(size_t *)dict_find(v2i, key)->data;
            adj[iv][jv] = 1;
            tok = strtok(0, ",");
        }
    }

    // compute cost of moving between non-zero valves and turning them on
    size_t(*cost)[n_lines] = calloc(n_lines, sizeof(*cost));
    list_insert_first(non_zero, COPY(AA));
    for (const Node *start = non_zero->first; start; start = start->next) {
        const size_t S = *(size_t *)start->data;
        for (const Node *end = non_zero->first; end; end = end->next) {
            const size_t E = *(size_t *)end->data;
            if ((S != E) && (E != AA)) {
                cost[S][E] = bfs(n_lines, *adj, S, E) + 1;
            }
        }
    }
    free(list_remove_first(non_zero));

    // part 1
    printf("%zu\n", dfs(n_lines, rate, non_zero, *cost, AA, 30, 1));

    // part 2
    printf("%zu\n", dfs(n_lines, rate, non_zero, *cost, AA, 26, 2));

    // cleanup
    lines_free(line, n_lines);
    dict_free(&v2i, free);
    free(rate);
    list_free(&non_zero, free);
    free(adj);
    free(cost);
}
