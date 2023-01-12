/*
 * Day 15: Beacon Exclusion Zone
 * (https://adventofcode.com/2022/day/15)
 *
 * Part 1:
 * - insead of marking points, create ranges
 * - reduce the ranges and exclude beacons that are inside of this range
 *
 * Part 2:
 * - missing beacon must be exactly +1 outside of range of one of the other beacons
 * - only check this diamond pattern around the beacon ranges
 */
#include "../aoc/aoc.h"

typedef struct Point {
    long x;
    long y;
    long d; // distance of sensor to beacon, if sensor, else 0
} Point;

long point_distance(const Point *a, const Point *b)
{
    return labs(b->x - a->x) + labs(b->y - a->y); // Manhattan distance
}

typedef struct Range {
    long x0;
    long x1;
} Range;

int range_cmp_x0(const void *a, const void *b)
{
    return ((Range *)a)->x0 - ((Range *)b)->x0;
}

size_t solve_part_1(const List *sensor, const Dict *beacon, const long y0)
{
    // for every sensor create a range of x values that it can see on specified line
    List *range = list_alloc(sizeof(Range));
    for (const Node *node = sensor->first; node; node = node->next) {
        const Point *ps = node->data;
        const long w = ps->d - labs(ps->y - y0);
        if (w >= 0) {
            Range r = { .x0 = ps->x - w, .x1 = ps->x + w };
            list_insert_last(range, COPY(r));
        }
    }

    // reduce ranges
    list_sort(range, range_cmp_x0);
    for (Node *node = range->first->next; node; node = node->next) {
        Node *prev = node->prev;
        Range *r0 = prev->data;
        Range *r1 = node->data;
        if (r0->x1 >= r1->x0) { // ranges overlap
            r1->x0 = MIN(r1->x0, r0->x0);
            r1->x1 = MAX(r1->x1, r0->x1);
            free(list_delete(range, prev));
        }
    }

    // add up length of ranges
    size_t count = 0;
    for (const Node *node = range->first; node; node = node->next) {
        const Range *r = node->data;
        count += (r->x1 - r->x0);
    }

    // remove all position where beacons are
    for (size_t i = 0; i < beacon->len; ++i) {
        const Item *item = &beacon->item[i];
        while (item && item->key) {
            const Point *p = item->data;
            for (const Node *node = range->first; node; node = node->next) {
                const Range *r = node->data;
                if ((p->y == y0) && (r->x0 <= p->x) && (p->x <= r->x1)) {
                    --count;
                    break;
                }
            }
            item = item->next;
        }
    }

    // cleanup
    list_free(&range, free);

    return count;
}

size_t solve_part_2(const List *sensor, const long clip[2])
{
    // missing beacon must be +1 outside of the range of one of the sensors
    // find it by only checking these points
    for (const Node *node0 = sensor->first; node0; node0 = node0->next) {
        const Point *ps0 = node0->data;

        // create start points that are just out of range
        const Point start[4] = {
            { .x = ps0->x, .y = ps0->y - ps0->d - 1 }, // up
            { .x = ps0->x + ps0->d + 1, .y = ps0->y }, // right
            { .x = ps0->x, .y = ps0->y + ps0->d + 1 }, // down
            { .x = ps0->x - ps0->d - 1, .y = ps0->y }, // left
        };

        // check diamond pattern
        const long dx[4] = { +1, -1, -1, +1 };
        const long dy[4] = { +1, +1, -1, -1 };
        for (size_t i = 0; i < 4; ++i) {
            for (long j = 0; j < ps0->d + 1; ++j) {
                // create candidate point
                const Point p = {
                    .x = start[i].x + j * dx[i],
                    .y = start[i].y + j * dy[i],
                };
                if ((p.x < clip[0]) || (clip[1] < p.x) || (p.y < clip[0]) || (clip[1] < p.y)) {
                    continue;
                }

                // check if it is visble from one of the sensors
                long visible = 0;
                for (const Node *node1 = sensor->first; node1; node1 = node1->next) {
                    const Point *ps1 = node1->data;
                    if (point_distance(ps1, &p) <= ps1->d) {
                        visible = 1;
                        break;
                    }
                }
                if (!visible) {
                    return p.x * 4000000 + p.y;
                }
            }
        }
    }
    assert(!"Beacon not found.");
}

int main(void)
{
    // read input
    const char **line = 0;
    const size_t n_lines = lines_read(&line, "15.txt");

    // create list of sensors and dict of beacons
    List *sensor = list_alloc(sizeof(Point));
    Dict *beacon = dict_alloc(sizeof(Point), 100);
    char key[256] = "";
    for (size_t i = 0; i < n_lines; ++i) {
        Point ps = { 0 };
        Point pb = { 0 };
        sscanf(line[i], "Sensor at x=%ld, y=%ld: closest beacon is at x=%ld, y=%ld",
            &ps.x, &ps.y, &pb.x, &pb.y);
        ps.d = point_distance(&ps, &pb);
        list_insert_last(sensor, COPY(ps));
        free(dict_insert(beacon, KEY(key, "%ld,%ld", pb.x, pb.y), COPY(pb)));
    }

    // part 1
    printf("%zu\n", solve_part_1(sensor, beacon, 2000000));

    // part 2
    printf("%zu\n", solve_part_2(sensor, (long[]) { 0, 4000000 }));

    // cleanup
    lines_free(line, n_lines);
    list_free(&sensor, free);
    dict_free(&beacon, free);
}
