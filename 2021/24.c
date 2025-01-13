/*
 * Day 24: Arithmetic Logic Unit
 * (https://adventofcode.com/2021/day/24)
 *
 * Part 1:
 * - notice that same function is run on each input digit
 *   - rewrite it in c as alu_chunk_func
 * - keep track of all possible z values and prune the result to get down to a final
 *   value of 0
 * - print max value
 *
 * Part 2:
 * - print min value
 */
#include "aoc.h"

#define NI 14  // number of sections
#define NJ 18  // number of lines per section

typedef struct Parameters {
    long a, b, c;
} Parameters;

long alu_chunk_func(Parameters param, long z, long w) {
    if (z % 26 + param.b != w)
        return ((z / param.a) * 26) + w + param.c;
    else
        return z / param.a;
}

int main(void) {
    // read input
    const char **line = 0;
    const size_t n_lines = lines_read(&line, "2021/input/24.txt");

    // read parameters
    Parameters Param[NI] = {0};
    for (size_t i = 0, iline = 0; i < NI; ++i) {
        for (size_t j = 0; j < NJ; ++j, ++iline) {
            if (j == 4) sscanf(line[iline], "%*s %*s %ld", &Param[i].a);
            if (j == 5) sscanf(line[iline], "%*s %*s %ld", &Param[i].b);
            if (j == 15) sscanf(line[iline], "%*s %*s %ld", &Param[i].c);
        }
    }

    // find smallest and largest model number
    Dict *z_values = dict_alloc(sizeof(long[2]), 1000000);
    dict_insert(z_values, "0", memdup((long[2]){0, 0}, sizeof(long[2])));
    char key[256] = "";
    for (const Parameters *param = Param; param < Param + NI; ++param) {
        // create temporary z_values dict
        Dict *z_values_new = dict_alloc(sizeof(long[2]), 1000000);
        // iterate through current z_values
        for (size_t i = 0; i < z_values->size; ++i) {
            for (const Item *z_item = &z_values->item[i]; z_item && z_item->key;
                 z_item = z_item->next) {
                // extract current z and min/max
                const long z = strtol(z_item->key, 0, 10);
                const long min = ((long *)z_item->data)[0];
                const long max = ((long *)z_item->data)[1];
                // go through all possible digits
                for (long digit = 1; digit <= 9; ++digit) {
                    // compute new z value
                    const long z_new = alu_chunk_func(*param, z, digit);
                    // check if we care about the new z value
                    if ((param->a == 1) || ((param->a == 26) && (z_new < z))) {
                        const long min_new = min * 10 + digit;
                        const long max_new = max * 10 + digit;
                        // update existing or insert new z value
                        Item *z_item_new = dict_find(z_values_new, KEY(key, "%ld", z_new));
                        if (z_item_new) {
                            const long min_prev = ((long *)z_item_new->data)[0];
                            const long max_prev = ((long *)z_item_new->data)[1];
                            ((long *)z_item_new->data)[0] = MIN(min_prev, min_new);
                            ((long *)z_item_new->data)[1] = MAX(max_prev, max_new);
                        }
                        else {
                            dict_insert(z_values_new, key,
                                        memdup((long[2]){min_new, max_new}, sizeof(long[2])));
                        }
                    }
                }
            }
        }
        dict_free(&z_values, free);
        z_values = z_values_new;
    }

    // part 1
    printf("%ld\n", ((long *)dict_find(z_values, "0")->data)[1]);

    // part 2
    printf("%ld\n", ((long *)dict_find(z_values, "0")->data)[0]);

    // cleanup
    lines_free(line, n_lines);
    dict_free(&z_values, free);
}
