#include "aoc.h"

int main(void)
{
    defer(array_clear) Array lines = read_lines("2024/input/01.txt");
    smart long *left = calloc(lines.size, sizeof(*left));
    smart long *right = calloc(lines.size, sizeof(*right));
    for (long i = 0; i < lines.size; ++i)
        sscanf(lines.item[i].data, "%ld %ld", &left[i], &right[i]);

    // part one
    qsort(left, lines.size, sizeof(*left), longcmp);
    qsort(right, lines.size, sizeof(*right), longcmp);
    long sum1 = 0;
    for (long i = 0; i < lines.size; ++i) sum1 += labs(left[i] - right[i]);
    printf("%ld\n", sum1);

    // part two
    defer(hmap_clear) Hmap lcount = hmap_create_default(lines.size, sizeof(long));
    defer(hmap_clear) Hmap rcount = hmap_create_default(lines.size, sizeof(long));
    for (long i = 0, *cnt; i < lines.size; ++i) {
        if ((cnt = hmap_insert(&lcount, keyprint("%ld", left[i]), (long[]){1}, 1))) *cnt += 1;
        if ((cnt = hmap_insert(&rcount, keyprint("%ld", right[i]), (long[]){1}, 1))) *cnt += 1;
    }
    long sum2 = 0;
    HmapForEach(item, &lcount)
    {
        const long lval = strtol(item->key, 0, 10);
        const long *lcnt = item->data;
        const long *rcnt = hmap_find(&rcount, item->key);
        if (!rcnt) continue;
        sum2 += lval * *lcnt * *rcnt;
    }
    printf("%ld\n", sum2);
}
