/*
 * Day 18: Snailfish
 * (https://adventofcode.com/2021/day/18)
 *
 * Part 1:
 * - pairs are like binary trees
 * - form the input pairs, keep track of the parant of each leaf
 * - sum up the pairs, according to the rules
 * - compute the magnitude
 *
 * Part 2:
 * - create each input pair combination
 * - sum them up
 * - keep track of the largest magnitude
 */
#include "aoc.h"

typedef enum ElementType {
    ET_NUMBER,
    ET_PAIR,
} ElementType;

typedef struct Pair {
    struct Pair *parent;
    ElementType x_type;
    union {
        long x_num;
        struct Pair *x_pair;
    };
    ElementType y_type;
    union {
        long y_num;
        struct Pair *y_pair;
    };
} Pair;

Pair *pair_alloc(const char **line_ptr, Pair *parent)
{
    const char *line = *line_ptr;
    assert(*line == '[');

    Pair *pair = calloc(1, sizeof(*pair));
    pair->parent = parent;

    // read x
    if (isdigit(*(++line))) {
        pair->x_type = ET_NUMBER;
        sscanf(line, "%ld", &pair->x_num);
        line = strchr(line, ',');
        assert(line);
    } else {
        pair->x_type = ET_PAIR;
        pair->x_pair = pair_alloc(&line, pair);
    }

    // read y
    if (isdigit(*(++line))) {
        pair->y_type = ET_NUMBER;
        sscanf(line, "%ld", &pair->y_num);
        line = strchr(line, ']');
        assert(line);
    } else {
        pair->y_type = ET_PAIR;
        pair->y_pair = pair_alloc(&line, pair);
    }

    *line_ptr = line + 1;
    return pair;
}

void pair_free(Pair *pair)
{
    if (pair->x_type == ET_PAIR) {
        pair_free(pair->x_pair);
    }
    if (pair->y_type == ET_PAIR) {
        pair_free(pair->y_pair);
    }
    free(pair);
}

long *get_num_right_down(Pair *pair);
long *get_num_left_up(Pair *pair);
long *get_num_left_down(Pair *pair);
long *get_num_right_up(Pair *pair);

long *get_num_right_down(Pair *pair)
{
    if (pair->y_type == ET_NUMBER) {
        return &pair->y_num;
    } else {
        return get_num_right_down(pair->y_pair);
    }
}

long *get_num_left_up(Pair *pair)
{
    Pair *parent = pair->parent;
    if (parent) {
        if ((parent->y_type == ET_PAIR) && (parent->y_pair == pair)) {
            if (parent->x_type == ET_NUMBER) {
                return &parent->x_num;
            } else {
                return get_num_right_down(parent->x_pair);
            }
        } else {
            return get_num_left_up(parent);
        }
    }
    return 0;
}

long *get_num_left_down(Pair *pair)
{
    if (pair->x_type == ET_NUMBER) {
        return &pair->x_num;
    } else {
        return get_num_left_down(pair->x_pair);
    }
}

long *get_num_right_up(Pair *pair)
{
    Pair *parent = pair->parent;
    if (parent) {
        if ((parent->x_type == ET_PAIR) && (parent->x_pair == pair)) {
            if (parent->y_type == ET_NUMBER) {
                return &parent->y_num;
            } else {
                return get_num_left_down(parent->y_pair);
            }
        } else {
            return get_num_right_up(parent);
        }
    }
    return 0;
}

int pair_explode(Pair *pair, size_t depth)
{
    if ((depth < 4)) {
        int did_explode = 0;
        if (pair->x_type == ET_PAIR) {
            did_explode |= pair_explode(pair->x_pair, depth + 1);
        }
        if (!did_explode && (pair->y_type == ET_PAIR)) {
            did_explode |= pair_explode(pair->y_pair, depth + 1);
        }
        return did_explode;

    } else {
        assert(pair->x_type == ET_NUMBER);
        long *num_left = get_num_left_up(pair);
        if (num_left) {
            *num_left += pair->x_num;
        }

        assert(pair->y_type == ET_NUMBER);
        long *num_right = get_num_right_up(pair);
        if (num_right) {
            *num_right += pair->y_num;
        }

        if ((pair->parent->x_type == ET_PAIR) && (pair->parent->x_pair == pair)) {
            pair->parent->x_type = ET_NUMBER;
            pair->parent->x_num = 0;
        } else {
            pair->parent->y_type = ET_NUMBER;
            pair->parent->y_num = 0;
        }

        pair_free(pair);
        return 1;
    }
}

Pair *num_to_pair(long num)
{
    Pair *pair = calloc(1, sizeof(*pair));
    pair->x_type = ET_NUMBER;
    pair->x_num = num / 2;
    pair->y_type = ET_NUMBER;
    pair->y_num = num / 2 + num % 2;
    return pair;
}

int pair_split(Pair *pair)
{
    int did_split = 0;
    if (pair->x_type == ET_PAIR) {
        did_split |= pair_split(pair->x_pair);
    } else {
        if (pair->x_num >= 10) {
            pair->x_type = ET_PAIR;
            pair->x_pair = num_to_pair(pair->x_num);
            pair->x_pair->parent = pair;
            return 1;
        }
    }
    if (did_split) {
        return did_split;
    }
    if (pair->y_type == ET_PAIR) {
        did_split |= pair_split(pair->y_pair);
    } else {
        if (pair->y_num >= 10) {
            pair->y_type = ET_PAIR;
            pair->y_pair = num_to_pair(pair->y_num);
            pair->y_pair->parent = pair;
            return 1;
        }
    }
    return did_split;
}

void pair_reduce(Pair *pair)
{
    if (pair_explode(pair, 0)) {
        pair_reduce(pair);
    }
    if (pair_split(pair)) {
        pair_reduce(pair);
    }
}

Pair *pair_add(Pair *left, Pair *right)
{
    Pair *sum = calloc(1, sizeof(*sum));

    sum->x_type = ET_PAIR;
    sum->x_pair = left;
    left->parent = sum;

    sum->y_type = ET_PAIR;
    sum->y_pair = right;
    right->parent = sum;

    pair_reduce(sum);
    return sum;
}

long pair_magnitude(const Pair *pair)
{
    long mag = 0;
    if (pair->x_type == ET_PAIR) {
        mag += 3 * pair_magnitude(pair->x_pair);
    } else {
        mag += 3 * pair->x_num;
    }
    if (pair->y_type == ET_PAIR) {
        mag += 2 * pair_magnitude(pair->y_pair);
    } else {
        mag += 2 * pair->y_num;
    }
    return mag;
}

int main(void)
{
    // read input
    const char **line = 0;
    const size_t n_lines = lines_read(&line, "2021/input/18.txt");

    // sum pairs
    Pair *sum = pair_alloc(&(const char *) { line[0] }, 0);
    for (size_t i = 1; i < n_lines; ++i) {
        sum = pair_add(sum, pair_alloc(&(const char *) { line[i] }, 0));
    }

    // part 1
    printf("%ld\n", pair_magnitude(sum));

    // greates sum of two pairs
    long max_sum = 0;
    for (size_t i = 0; i < n_lines; ++i) {
        for (size_t j = 0; j < n_lines; ++j) {
            if (i != j) {
                Pair *a = pair_alloc(&(const char *) { line[i] }, 0);
                Pair *b = pair_alloc(&(const char *) { line[j] }, 0);
                Pair *ab = pair_add(a, b);
                max_sum = MAX(max_sum, pair_magnitude(ab));
                pair_free(ab);
            }
        }
    }

    // part 2
    printf("%ld\n", max_sum);

    // cleanup
    lines_free(line, n_lines);
    pair_free(sum);
}
