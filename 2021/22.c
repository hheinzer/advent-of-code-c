/*
 * Day 22: Reactor Reboot
 * (https://adventofcode.com/2021/day/22)
 *
 * Part 1:
 * - the strategy is to compute intersections of the cube to insert with all
 *   cubes that are already there and insert them with inverse sign of the cube
 *   that is already in the list
 * - only insert cubes that are between -50 and 50
 * - sum up all the volumes with their sign
 *
 * Part 2:
 * - insert all cubes
 */
#include "aoc.h"

typedef struct Cube {
    long x0;
    long x1;
    long y0;
    long y1;
    long z0;
    long z1;
    long sign;
} Cube;

Cube *cube_alloc(const char *line, long limit)
{
    // create cube
    Cube cube = { 0 };
    char cmd[4] = "";
    sscanf(line, "%3s x=%ld..%ld,y=%ld..%ld,z=%ld..%ld",
        cmd, &cube.x0, &cube.x1, &cube.y0, &cube.y1, &cube.z0, &cube.z1);

    // apply limit if it exists
    if (limit) {
        if ((labs(cube.x0) > limit) || (labs(cube.x1) > limit)) return 0;
        if ((labs(cube.y0) > limit) || (labs(cube.y1) > limit)) return 0;
        if ((labs(cube.z0) > limit) || (labs(cube.z1) > limit)) return 0;
    }

    // check cube
    assert(cube.x1 >= cube.x0);
    assert(cube.y1 >= cube.y0);
    assert(cube.z1 >= cube.z0);

    // set the sign of the cube
    if (!strcmp(cmd, "on")) {
        cube.sign = +1;
    } else {
        cube.sign = -1;
    }

    return memdup(&cube, sizeof(cube));
}

Cube *cube_intersection(const Cube *a, const Cube *b)
{
    // create intersecting cube
    Cube cube = { 0 };
    cube.x0 = MAX(a->x0, b->x0);
    cube.x1 = MIN(a->x1, b->x1);
    cube.y0 = MAX(a->y0, b->y0);
    cube.y1 = MIN(a->y1, b->y1);
    cube.z0 = MAX(a->z0, b->z0);
    cube.z1 = MIN(a->z1, b->z1);

    // check if the intersection was valid
    if (cube.x1 < cube.x0) return 0;
    if (cube.y1 < cube.y0) return 0;
    if (cube.z1 < cube.z0) return 0;

    return memdup(&cube, sizeof(cube));
}

void cube_insert(List *cubes, Cube *new)
{
    const size_t n = cubes->len;
    Node *node = cubes->first;
    for (size_t i = 0; i < n; ++i) {
        Cube *cube = node->data;

        // insert intersection with inverse sign
        Cube *intr = cube_intersection(cube, new);
        if (intr) {
            intr->sign = -cube->sign;
            list_insert_last(cubes, intr);
        }

        node = node->next;
    }

    // cubes that are subtracted are not added to the list
    if (new->sign == +1) {
        list_insert_last(cubes, new);
    } else {
        free(new);
    }
}

long cube_volume(const Cube *cube)
{
    return (cube->x1 - cube->x0 + 1) * (cube->y1 - cube->y0 + 1) * (cube->z1 - cube->z0 + 1);
}

size_t cube_sum_volume(const List *cubes)
{
    long vol = 0;
    for (const Node *node = cubes->first; node; node = node->next) {
        const Cube *cube = node->data;
        vol += cube->sign * cube_volume(cube);
    }
    assert(vol >= 0);
    return vol;
}

int main(void)
{
    // read input
    const char **line = 0;
    const size_t n_lines = lines_read(&line, "2021/input/22.txt");

    // create cubes
    List *cubes1 = list_alloc(sizeof(Cube));
    for (size_t i = 0; i < n_lines; ++i) {
        Cube *cube = cube_alloc(line[i], 50);
        if (cube) cube_insert(cubes1, cube);
    }

    // part 1
    printf("%zu\n", cube_sum_volume(cubes1));

    // create cubes
    List *cubes2 = list_alloc(sizeof(Cube));
    for (size_t i = 0; i < n_lines; ++i) {
        Cube *cube = cube_alloc(line[i], 0);
        if (cube) cube_insert(cubes2, cube);
    }

    // part 1
    printf("%zu\n", cube_sum_volume(cubes2));

    // cleanup
    lines_free(line, n_lines);
    list_free(&cubes1, free);
    list_free(&cubes2, free);
}
