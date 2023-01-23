/*
 * Day 20: Grove Positioning System
 * (https://adventofcode.com/2022/day/20)
 *
 * Part 1:
 * - set up a circular list for the values
 * - set up an array of pointers to the nodes, so that we can move them in order
 *
 * Part 2:
 * - multiply values before moving
 * - move 10 times
 */
#include "aoc.h"

CMP(long)

long solve(List *list, long mul, size_t n_mix)
{
    const size_t n = list->len;

    // create array of node pointers, multiply values with decryption key
    Node **node = calloc(n, sizeof(*node));
    for (size_t i = 0; i < n; ++i) {
        node[i] = list_get(list, i);
        *(long *)node[i]->data *= mul;
    }

    // make list circular
    list->first->prev = list->last;
    list->last->next = list->first;

    // get zero node
    const Node *zero = list_find(list, &(long) { 0 }, cmp_long_asc);

    // move nodes
    for (size_t mix = 0; mix < n_mix; ++mix) {
        for (size_t i = 0; i < n; ++i) {
            Node *curr = node[i];
            const long val = *(long *)curr->data;
            Node *prev = curr;
            if (val >= 0) { // move right
                for (size_t j = 0; j < labs(val) % (n - 1); ++j) {
                    prev = prev->next;
                }
            } else { // move left
                for (size_t j = 0; j < (labs(val) + 1) % (n - 1); ++j) {
                    prev = prev->prev;
                }
            }
            if (prev != curr) {
                curr->prev->next = curr->next;
                curr->next->prev = curr->prev;
                curr->prev = prev;
                curr->next = prev->next;
                prev->next->prev = curr;
                prev->next = curr;
            }
        }
    }

    // read code
    long sum = 0;
    const size_t offset[3] = { 1000, 2000, 3000 };
    for (size_t i = 0; i < 3; ++i) {
        const Node *curr = zero;
        for (size_t j = 0; j < offset[i] % n; ++j) {
            curr = curr->next;
        }
        sum += *(long *)curr->data;
    }

    // cleanup
    list_free(&list, free);
    free(node);

    return sum;
}

int main(void)
{
    // read input
    const char **line = 0;
    const size_t n = lines_read(&line, "2022/input/20.txt");

    // create initial list and array of pointer to the nodes
    List *list = list_alloc(sizeof(long));
    for (size_t i = 0; i < n; ++i) {
        long val = 0;
        sscanf(line[i], "%ld", &val);
        list_insert_last(list, COPY(val));
    }

    // part 1
    printf("%ld\n", solve(list_copy(list, memcpy), 1, 1));

    // part 2
    printf("%ld\n", solve(list_copy(list, memcpy), 811589153, 10));

    // cleanup
    lines_free(line, n);
    list_free(&list, free);
}
