/*
 * Day 11: Monkey in the Middle
 * (https://adventofcode.com/2022/day/11)
 *
 * Part 1:
 * - create the monkeys using a list for the held items
 *   - us pop_front and push_back to get and throw items
 *
 * Part 2:
 * - the trick to keep the worry levels from overflowing is to
 *   compute a combined modulo and use it to decrease to worry levels
 *   of every item
 *   - the combined modulo is computed by multiplying all test values
 *   - this will not change whether or not an item is divisible by the
 *     test value, keeping the test logic intact
 */
#include "aoc.c"

CMP(size_t)

typedef struct Monkey {
    List *item;
    char op_type;
    long op_value;
    long test_value;
    size_t if_true;
    size_t if_false;
    size_t inspect_count;
} Monkey;

void solve(const char **line, size_t n_lines, size_t part)
{
    // create monkeys
    Monkey monkey[8] = { 0 };
    for (size_t i = 0; i < n_lines; i += 7) {
        // read monkey index
        size_t imonkey = -1;
        sscanf(line[i + 0], " Monkey %zu:", &imonkey);

        // read items
        monkey[imonkey].item = list_create(sizeof(long));
        char *c = strchr(line[i + 1], ':');
        long item = 0;
        while (c && sscanf(c + 1, " %ld", &item)) {
            list_push_back_copy(monkey[imonkey].item, &item, memcpy);
            c = strchr(c + 1, ',');
        }

        // read operation
        char tmp[256] = "";
        sscanf(line[i + 2], " Operation: new = old %[^\n]", tmp);
        if (!strcmp(tmp, "* old")) {
            monkey[imonkey].op_type = '^';
            monkey[imonkey].op_value = 2;
        } else {
            monkey[imonkey].op_type = tmp[0];
            monkey[imonkey].op_value = strtol(tmp + 1, 0, 10);
        }

        // read test
        sscanf(line[i + 3], " Test: divisible by %ld", &monkey[imonkey].test_value);
        sscanf(line[i + 4], " If true: throw to monkey %zu", &monkey[imonkey].if_true);
        sscanf(line[i + 5], " If false: throw to monkey %zu", &monkey[imonkey].if_false);
    }

    // compute combined modulus, reduce magnitude of worry level
    long mod = 1;
    for (size_t i = 0; i < LEN(monkey); ++i) {
        mod *= monkey[i].test_value;
    }

    // simulate rounds
    size_t n_rounds = 0;
    switch (part) {
    case 1:
        n_rounds = 20;
        break;
    case 2:
        n_rounds = 10000;
        break;
    default:
        assert(!"Illegal part detected.");
    }
    for (size_t round = 0; round < n_rounds; ++round) {
        for (size_t i = 0; i < LEN(monkey); ++i) {
            while (monkey[i].item->nelem) {
                // inspect item
                long *item = list_pop_front(monkey[i].item);
                switch (monkey[i].op_type) {
                case '+':
                    *item += monkey[i].op_value;
                    break;
                case '-':
                    *item -= monkey[i].op_value;
                    break;
                case '*':
                    *item *= monkey[i].op_value;
                    break;
                case '/':
                    *item /= monkey[i].op_value;
                    break;
                case '^':
                    *item *= *item;
                    break;
                default:
                    assert(!"Illegal operation encountered.");
                }
                ++monkey[i].inspect_count;

                // modify worry level
                switch (part) {
                case 1:
                    *item /= 3;
                    break;
                case 2:
                    *item %= mod;
                    break;
                default:
                    break;
                }

                // test and throw item
                if (*item % monkey[i].test_value == 0) {
                    list_push_back(monkey[monkey[i].if_true].item, item);
                } else {
                    list_push_back(monkey[monkey[i].if_false].item, item);
                }
            }
        }
    }

    // sort inspection counts
    size_t inspect_count[LEN(monkey)] = { 0 };
    for (size_t i = 0; i < LEN(monkey); ++i) {
        inspect_count[i] = monkey[i].inspect_count;
    }
    qsort(inspect_count, LEN(monkey), sizeof(*inspect_count), cmp_size_t_dsc);

    // print monkey business
    printf("%zu\n", inspect_count[0] * inspect_count[1]);

    // cleanup
    for (size_t i = 0; i < LEN(monkey); ++i) {
        list_free(monkey[i].item);
    }
}

int main(void)
{
    // read input
    const char **line = 0;
    const size_t n_lines = lines_read(&line, "11.txt");

    // part 1
    solve(line, n_lines, 1);

    // part 2
    solve(line, n_lines, 2);

    // cleanup
    lines_free(line, n_lines);
}
