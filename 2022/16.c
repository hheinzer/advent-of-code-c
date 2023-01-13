/*
 * Day 16: Proboscidea Volcanium
 * (https://adventofcode.com/2022/day/16)
 *
 * Part 1:
 * - find all the non-zero flow rate valves, only they are interesting
 * - compute the cost (distances to + 1 minute open valve) from any valve to any other
 *   valve
 * - search all possible ways of visiting the valves and find the max flow rate
 *
 * Part 2:
 * - split the list of valves between me and the elephant in all possible ways
 * - search all possible ways of visiting the valves and find the max flow rate for me
 *   and the elephant, add up the rates
 */
#include "../aoc/aoc.h"

typedef struct Valve {
    long rate;
    List *adj;
} Valve;

Valve *valve_alloc(void)
{
    Valve valve = { .adj = list_alloc(sizeof(char[3])) };
    return COPY(valve);
}

void valve_free(Valve *valve)
{
    list_free(&valve->adj, free);
    free(valve);
}

int subcost_free(const char *key, Dict *subcost)
{
    (void)key;
    dict_free(&subcost, free);
    return 0;
}

size_t shortest_path_length(const Dict *valve, char *start, char *end)
{
    // prepare breadth first search (BFS)
    Queue *queue = queue_alloc(sizeof(char[3]));
    Dict *visited = dict_alloc(0, valve->size);
    Dict *dist = dict_alloc(sizeof(size_t), valve->size);

    // initialize BFS
    dict_insert(visited, start, 0);
    dict_insert(dist, start, COPY((size_t) { 0 }));
    queue_push(queue, start);

    // start BFS
    size_t length = 0;
    while (queue->len) {
        // get first valve
        const char *first_key = queue_pop(queue);
        const Valve *first = dict_find(valve, first_key)->data;
        const size_t *dist_first = dict_find(dist, first_key)->data;

        // check neighbors
        for (const Node *node = first->adj->first; node; node = node->next) {
            char *adj = node->data;
            if (!dict_find(visited, adj)) {
                if (!strcmp(adj, end)) {
                    length = *dist_first + 1;
                    goto cleanup;
                }
                dict_insert(visited, adj, 0);
                dict_insert(dist, adj, COPY((size_t) { *dist_first + 1 }));
                queue_push(queue, adj);
            }
        }
    }

cleanup:
    queue_free(&queue, 0);
    dict_free(&visited, 0);
    dict_free(&dist, free);

    return length;
}

int dict_key_print(const char *key)
{
    return !printf("%s ", key);
}

size_t _max_total_flow(const Dict *valve, const Dict *cost, const char *location, size_t time_allowed,
    Dict *visited, size_t time_taken, size_t total_flow)
{
    // add location to visited
    dict_insert(visited, location, 0);

    // create list of valves to be visited next
    const char *next_valve[valve->len];
    size_t n = 0;
    const Dict *subcost = dict_find(cost, location)->data;
    for (size_t i = 0; i < subcost->size; ++i) {
        const Item *item = &subcost->item[i];
        while (item && item->key) {
            const char *valve_key = item->key;

            // filter out valves that have been visited before (turned on, already)
            if (dict_find(visited, valve_key)) {
                goto skip;
            }

            // filter out valves that we cannot move to anymore
            const size_t *valve_cost = dict_find(subcost, valve_key)->data;
            if (time_taken + (*valve_cost) >= time_allowed) {
                goto skip;
            }

            // add valve
            next_valve[n++] = item->key;

        skip:
            item = item->next;
        }
    }

    // go to all next valves
    size_t max_total_flow = 0;
    for (size_t i = 0; i < n; ++i) {
        const char *valve_key = next_valve[i];
        const size_t *valve_cost = dict_find(subcost, valve_key)->data;
        const size_t valve_rate = ((Valve *)dict_find(valve, valve_key)->data)->rate;
        const size_t _total_flow = _max_total_flow(
            valve,
            cost,
            valve_key,
            time_allowed,
            visited,
            time_taken + (*valve_cost),
            total_flow + (time_allowed - time_taken - (*valve_cost)) * valve_rate);
        max_total_flow = MAX(max_total_flow, _total_flow);
    }

    // remove location from visited
    dict_remove(visited, location);

    return MAX(max_total_flow, total_flow);
}

size_t max_total_flow(const Dict *valve, const Dict *cost, Dict *visited, const char *location, size_t time_allowed)
{
    // initialize search
    size_t time_taken = 0;
    size_t total_flow = 0;

    // search for best strategy result
    const size_t max_total_flow = _max_total_flow(valve, cost, location, time_allowed,
        visited, time_taken, total_flow);

    return max_total_flow;
}

long ipow(long base, size_t exp)
{
    long ret = 1;
    for (size_t i = 0; i < exp; ++i) {
        ret *= base;
    }
    return ret;
}

long *product(long *a, size_t na, size_t nj)
{
    const size_t ni = ipow(na, nj);
    long(*product)[nj] = calloc(ni, sizeof(*product));
    for (size_t i = 0; i < ni; ++i) {
        for (size_t j = 0; j < nj; ++j) {
            product[i][j] = a[(i / (ni / ipow(na, j + 1))) % na];
        }
    }
    return *product;
}

int main(void)
{
    // read input
    const char **line = 0;
    const size_t n_lines = lines_read(&line, "16.txt");

    // build network
    Dict *valve = dict_alloc(sizeof(Valve), 60);
    List *non_zero = list_alloc(sizeof(char[3]));
    for (size_t i = 0; i < n_lines; ++i) {
        Valve *v = valve_alloc();
        char key[3] = "";
        char adj[256] = "";
        sscanf(line[i], "Valve %s has flow rate=%ld; %*s %*s to %*s %[^\n]",
            key, &v->rate, adj);
        dict_insert(valve, key, v);
        if (v->rate > 0) { // valve with non-zero flow rate
            list_insert_last(non_zero, COPY(key));
        }
        char *tok = strtok(adj, ",");
        while (tok) { // add adjacent valves
            sscanf(tok, " %s ", key);
            list_insert_last(v->adj, COPY(key));
            tok = strtok(0, ",");
        }
    }

    // compute distances of moving between non-zero rooms and turn on valve
    Dict *cost = dict_alloc(sizeof(Dict), valve->size);
    list_insert_last(non_zero, COPY("AA"));
    for (const Node *start = non_zero->first; start; start = start->next) {
        Dict *subcost = dict_alloc(sizeof(size_t), valve->size);
        for (const Node *end = non_zero->first; end; end = end->next) {
            if ((end != start) && strcmp(end->data, "AA")) {
                const size_t length = shortest_path_length(valve, start->data, end->data);
                dict_insert(subcost, end->data, COPY((size_t) { length + 1 }));
            }
        }
        dict_insert(cost, start->data, subcost);
    }
    free(list_remove_last(non_zero));

    // part 1
    Dict *visited = dict_alloc(0, valve->size);
    printf("%zu\n", max_total_flow(valve, cost, visited, "AA", 30));
    dict_free(&visited, 0);

    // create all possible combination patterns
    const size_t nj = non_zero->len;
    const size_t ni = ipow(2, nj);
    long(*prod)[nj] = TENSOR(prod, product((long[]) { 0, 1 }, 2, nj));

    // search for max flow with two players
    Dict *visited_me = dict_alloc(0, valve->size);
    Dict *visited_elephant = dict_alloc(0, valve->size);
    size_t max_total_flow_comb = 0;
    for (size_t i = 0, j; i < ni; ++i) {
        // create visited dicts for me and elephant
        j = 0;
        for (const Node *node = non_zero->first; node; node = node->next) {
            if (prod[i][j++]) {
                dict_insert(visited_me, node->data, 0);
            } else {
                dict_insert(visited_elephant, node->data, 0);
            }
        }

        // compute combined total flow
        size_t total_flow_comb = 0;
        total_flow_comb += max_total_flow(valve, cost, visited_me, "AA", 26);
        total_flow_comb += max_total_flow(valve, cost, visited_elephant, "AA", 26);
        max_total_flow_comb = MAX(max_total_flow_comb, total_flow_comb);

        // clear visited dicts
        j = 0;
        for (const Node *node = non_zero->first; node; node = node->next) {
            dict_remove(visited_me, node->data);
            dict_remove(visited_elephant, node->data);
        }
    }
    dict_free(&visited_me, 0);
    dict_free(&visited_elephant, 0);

    // part 2
    printf("%zu\n", max_total_flow_comb);

    // cleanup
    lines_free(line, n_lines);
    dict_free(&valve, valve_free);
    list_free(&non_zero, free);
    dict_traverse(cost, subcost_free);
    dict_free(&cost, 0);
    free(prod);
}
