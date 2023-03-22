/*
 * Day 23: Amphipod
 * (https://adventofcode.com/2021/day/23)
 *
 * Part 1:
 * - represent the burrow as one string (hallway + 1st row of rooms + 2nd row of rooms ...)
 * - use dijkstra search to find the minimum energy solution
 *
 * Part 2:
 * - insert two new lines and repeat
 */
#include "aoc.h"

#define NH 11 // maximum number of hallway spots
#define NR 4 // maximum number of rooms
#define NS 4 // maximum number of spots per room
#define NB (NH + NR * NS + 1)
#define NM (NR * NS * (NH + NR * NS)) // maximum number of moves

static const long energy[] = { ['A'] = 1, ['B'] = 10, ['C'] = 100, ['D'] = 1000 };
static const size_t stepout[] = { ['A'] = 2, ['B'] = 4, ['C'] = 6, ['D'] = 8 };
static const size_t parc[] = { 0, 1, 3, 5, 7, 9, 10 };
static const char solution[NB] = "...........ABCDABCDABCDABCD";

int burrow_is_blocked(size_t a, size_t b, const char *burrow)
{
    if (a < b) {
        for (size_t p = a + 1; p <= b; ++p) {
            if (burrow[p] != '.') return 1;
        }
        return 0;
    } else {
        for (size_t p = b; p < a; ++p) {
            if (burrow[p] != '.') return 1;
        }
        return 0;
    }
}

size_t burrow_can_enter_room(size_t a, size_t b, char amph, size_t nt, const size_t target[4],
    const char *burrow)
{
    size_t best_pos = SIZE_MAX;
    for (const size_t *t = target; t < target + nt; ++t) {
        if (burrow[*t] == '.') {
            best_pos = *t;
        } else if (burrow[*t] != amph) {
            return SIZE_MAX;
        }
    }
    if (!burrow_is_blocked(a, b, burrow)) return best_pos;
    return SIZE_MAX;
}

size_t burrow_can_leave_room(size_t nt, const size_t target[4], const char *burrow)
{
    for (const size_t *t = target; t < target + nt; ++t) {
        if (burrow[*t] == '.') continue;
        return *t;
    }
    return SIZE_MAX;
}

size_t burrow_possible_moves(size_t move[NM][2], const char *burrow, size_t nt)
{
    static const size_t target[][4] = {
        ['A'] = { 11, 15, 19, 23 },
        ['B'] = { 12, 16, 20, 24 },
        ['C'] = { 13, 17, 21, 25 },
        ['D'] = { 14, 18, 22, 26 },
    };
    size_t nm = 0;

    // generate moves for amphipods in hallway
    for (const size_t *a = parc; a < parc + LEN(parc); ++a) {
        const char amph = burrow[*a];
        if (amph == '.') continue;
        const size_t b = burrow_can_enter_room(
            *a, stepout[(size_t)amph], amph, nt, target[(size_t)amph], burrow);
        if (b == SIZE_MAX) continue;
        memcpy(move[nm++], (size_t[2]) { *a, b }, sizeof(size_t[2]));
    }

    // generate moves for amphipods in rooms
    for (size_t room = 'A'; room <= 'D'; ++room) {
        const size_t a = burrow_can_leave_room(nt, target[room], burrow);
        if (a == SIZE_MAX) continue;
        for (const size_t *b = parc; b < parc + LEN(parc); ++b) {
            if (burrow[*b] != '.') continue;
            if (burrow_is_blocked(stepout[room], *b, burrow)) continue;
            memcpy(move[nm++], (size_t[2]) { a, *b }, sizeof(size_t[2]));
        }
    }

    return nm;
}

long burrow_distance(size_t a, size_t b)
{
    const long p = MIN(a, b);
    const long r = MAX(a, b);
    return labs((long)stepout[(size_t)solution[r]] - p) + (r - 7) / 4;
}

char *burrow_move(const char *burrow, size_t a, size_t b)
{
    char *new_burrow = strdup(burrow);
    new_burrow[a] = burrow[b];
    new_burrow[b] = burrow[a];
    return new_burrow;
}

long dijkstra(const char *burrow)
{
    // compute the number of spots per room
    const size_t nb = strlen(burrow);
    const size_t nt = (nb - NH) / 4;

    // initialize dijkstra
    Heap *heap = heap_alloc(NB * sizeof(*burrow), 35000);
    Dict *seen = dict_alloc(sizeof(long), 1000000);

    // insert first state
    long cost = 0;
    const char *state = burrow;
    heap_insert(heap, cost, strdup(state));
    dict_insert(seen, state, memdup(&cost, sizeof(cost)));

    while (heap->len) {
        // get lowest energy state
        const Elem *elem = heap_peek(heap);
        cost = -elem->priority;
        state = elem->data;
        if (!strncmp(state, solution, nb)) {
            goto cleanup;
        }

        // insert all possible moves
        size_t move[NM][2] = { 0 };
        const size_t nm = burrow_possible_moves(move, state, nt);
        for (size_t m = 0; m < nm; ++m) {
            // create new state
            const size_t a = move[m][0];
            const size_t b = move[m][1];
            const long distance = burrow_distance(a, b);
            const long new_cost = cost + distance * energy[(size_t)state[a]];
            char *new_state = burrow_move(state, a, b);

            // filter out new states that already exist and have a higher cost
            const Item *item = dict_find(seen, new_state);
            if (item && (*(long *)item->data <= new_cost)) {
                free(new_state);
                continue;
            }

            // insert new state
            free(dict_insert(seen, new_state, memdup(&new_cost, sizeof(new_cost))));
            heap_insert(heap, -new_cost, new_state);
        }

        free(heap_remove(heap));
    }

cleanup:
    heap_free(&heap, free);
    dict_free(&seen, free);
    return cost;
}

int main(void)
{
    // read input
    const char **line = 0;
    const size_t n_lines = lines_read(&line, "2021/input/23.txt");

    // create initial burrow
    char burrow[NB] = "";
    memcpy(burrow, &line[1][1], NH * sizeof(*burrow));
    for (size_t s = 0; s < 2; ++s) {
        for (size_t r = 0; r < NR; ++r) {
            burrow[NH + s * NR + r] = line[2 + s][3 + 2 * r];
        }
    }

    // part 1
    printf("%ld\n", dijkstra(burrow));

    // update starting position
    memcpy(&burrow[NH + 3 * NR], &burrow[NH + NR], 4);
    memcpy(&burrow[NH + NR], "DCBADBAC", 8);

    // part 2
    printf("%ld\n", dijkstra(burrow));

    // cleanup
    lines_free(line, n_lines);
}
