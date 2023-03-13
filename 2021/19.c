/*
 * Day 19: Beacon Scanner
 * (https://adventofcode.com/2021/day/19)
 *
 * Part 1:
 * - parse scanners into array
 * - align all scanners with first scanner
 *   - check for matches one dimension at a time
 *   - early bail if there are less than 12 matches in any of the dimensions
 * - keep track of all global beacons with a field dict
 * - count the number of entries in field
 *
 * Part 2:
 * - compute all possible Manhatten distances of scanner origins
 * - take max
 */
#include "aoc.h"

typedef struct Scanner {
    size_t nb;
    long origin[3];
    long (*beacon)[3];
} Scanner;

typedef struct Pair {
    long value;
    size_t count;
} Pair;

Pair most_common(long *a, size_t n)
{
    size_t count_max = 0;
    const char *key_max = 0;
    char key[256] = "";
    Dict *dict = dict_alloc(sizeof(size_t), 2 * n);
    for (size_t i = 0; i < n; ++i) {
        KEY(key, "%ld", a[i]);
        Item *item = dict_find(dict, key);
        if (item) {
            size_t count = ++(*(size_t *)item->data);
            if (count > count_max) {
                count_max = count;
                key_max = item->key;
            }
        } else {
            size_t count = 1;
            dict_insert(dict, key, memdup(&count, sizeof(count)));
        }
    }
    Pair ret = { .count = count_max };
    sscanf(key_max, "%ld", &ret.value);
    dict_free(&dict, free);
    return ret;
}

Scanner *scanner_try_align(const Scanner *aligned, Scanner *candidate)
{
    static const long D[] = { 0, 1, 2, 0, 1, 2 };
    static const long s[] = { 1, 1, 1, -1, -1, -1 };
    const size_t na = aligned->nb;
    const size_t nc = candidate->nb;

    // initialize potentially aligned scanner
    Scanner updated = {
        .nb = nc,
        .beacon = calloc(nc, sizeof(long[3])),
    };

    // try to align candidate
    Scanner *ret = candidate;
    long x[na];
    size_t c = 0;
    long v = 0;
    for (size_t dim = 0; dim < 3; ++dim) {
        // copy all coordinates of one dimension from aligned
        for (size_t i = 0; i < na; ++i) {
            x[i] = aligned->beacon[i][dim];
        }

        // try to match it with all coordinate combinations of candidate
        long t[nc];
        long d = -1;
        long dp = -1;
        long dpp = -1;
        for (size_t i = 0; i < 6; ++i) {
            d = D[i];
            if ((d == dp) || (d == dpp)) {
                continue;
            }

            // permutate candidate beacon coordinates
            for (size_t j = 0; j < nc; ++j) {
                t[j] = s[i] * candidate->beacon[j][d];
            }

            // compute all possible differences to aligned beacon coordinates
            long w[na][nc];
            for (size_t j = 0; j < na; ++j) {
                for (size_t k = 0; k < nc; ++k) {
                    w[j][k] = t[k] - x[j];
                }
            }

            // get the most common difference
            Pair pair = most_common(*w, na * nc);
            v = pair.value;
            c = pair.count;
            if (c >= 12) {
                break;
            }
        }

        // check if there were at least 12 matches
        if (c < 12) {
            ret = 0;
            goto cleanup;
        }
        dpp = dp;
        dp = d;

        // update candidate origin and coordinates
        updated.origin[dim] = v;
        for (size_t i = 0; i < nc; ++i) {
            updated.beacon[i][dim] = t[i] - v;
        }
    }

    // update candidate
    memcpy(candidate->origin, updated.origin, sizeof(long[3]));
    memcpy(candidate->beacon, updated.beacon, nc * sizeof(long[3]));

cleanup:
    free(updated.beacon);
    return ret;
}

size_t manhatten(long a[3], long b[3])
{
    return labs(a[0] - b[0]) + labs(a[1] - b[1]) + labs(a[2] - b[2]);
}

int main(void)
{
    // read input
    const char **line = 0;
    const size_t n_lines = lines_read(&line, "2021/input/19.txt");

    // create scanners
    size_t nb_tot = 0;
    size_t ns = 0;
    Scanner *scanner = 0;
    for (size_t i = 0; i < n_lines; ++i) {
        if (!line[i][0]) { // empty line
            continue;
        }
        if (!strncmp(line[i], "---", 3)) { // beginning of a new scanner
            scanner = realloc(scanner, (++ns) * sizeof(*scanner));
            memset(&scanner[ns - 1], 0, sizeof(*scanner));
            continue;
        }

        // append beacon coordinates
        ++nb_tot;
        const size_t nb = ++scanner[ns - 1].nb;
        scanner[ns - 1].beacon = realloc(scanner[ns - 1].beacon, nb * sizeof(long[3]));
        long *beacon = scanner[ns - 1].beacon[nb - 1];
        memset(beacon, 0, sizeof(long[3]));
        sscanf(line[i], "%ld,%ld,%ld", &beacon[0], &beacon[1], &beacon[2]);
    }

    // align scanners
    Dict *field = dict_alloc(0, 2 * nb_tot);
    List *next = list_alloc(sizeof(Scanner *));
    List *rest = list_alloc(sizeof(Scanner *));
    list_insert_last(next, &scanner[0]);
    for (size_t s = 1; s < ns; ++s) {
        list_insert_last(rest, &scanner[s]);
    }
    char key[256] = "";
    while (next->len) {
        Scanner *aligned = list_remove_first(next);

        // try to align all scanners in rest
        List *new_rest = list_alloc(sizeof(Scanner *));
        for (Node *node = rest->first; node; node = node->next) {
            Scanner *candidate = node->data;
            Scanner *updated = scanner_try_align(aligned, candidate);
            if (updated) { // candidate was aligned
                list_insert_last(next, updated);
            } else { // candidate was not aligned
                list_insert_last(new_rest, candidate);
            }
        }
        list_free(&rest, 0);
        rest = new_rest;

        // insert beacon coordinates into global field
        for (size_t i = 0; i < aligned->nb; ++i) {
            KEY(key, "%ld,%ld,%ld",
                aligned->beacon[i][0], aligned->beacon[i][1], aligned->beacon[i][2]);
            dict_insert(field, key, 0);
        }
    }

    // part 1
    printf("%zu\n", field->len);

    // compute the largest distance between two scanners
    size_t dist_max = 0;
    for (size_t i = 0; i < ns; ++i) {
        for (size_t j = 0; j < ns; ++j) {
            dist_max = MAX(dist_max, manhatten(scanner[i].origin, scanner[j].origin));
        }
    }

    // part 2
    printf("%zu\n", dist_max);

    // cleanup
    lines_free(line, n_lines);
    for (size_t s = 0; s < ns; ++s) {
        free(scanner[s].beacon);
    }
    free(scanner);
    dict_free(&field, 0);
    list_free(&next, 0);
    list_free(&rest, 0);
}
