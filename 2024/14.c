#include "aoc.h"

typedef struct {
    Vec2 p;
    Vec2 v;
} Robot;

void parse(List *robots, const char *fname);
Vec2 position(const Robot *robot, long cols, long rows, long time);
long quadrant(Vec2 p, long cols, long rows);
long safety(const List *robots, long cols, long rows, long time);

int main(void) {
    Arena arena = arena_create(1 << 20);

    List robots = list_create(&arena, sizeof(Robot), 0);
    parse(&robots, "2024/input/14.txt");

    long cols = 101;
    long rows = 103;

    printf("%ld\n", safety(&robots, cols, rows, 100));

    long part2[2] = {LONG_MAX, 0};
    for (long time = 1; time <= rows * cols; time++) {
        long factor = safety(&robots, cols, rows, time);
        if (factor < part2[0]) {
            part2[0] = factor;
            part2[1] = time;
        }
    }
    printf("%ld\n", part2[1]);

    // Grid grid = grid_create(rows, cols, 0, &arena);
    // list_for_each(item, &robots) {
    //     Vec2 p = position(item->data, cols, rows, part2[1]);
    //     grid_set(&grid, p.r, p.c, -1);
    // }
    // grid_write_pgm(&grid, "2024/14.pgm");

    arena_destroy(&arena);
}

void parse(List *robots, const char *fname) {
    FILE *file = fopen(fname, "r");
    assert(file);
    char line[256];
    while (fgets(line, sizeof(line), file)) {
        Robot robot;
        sscanf(line, "p=%ld,%ld v=%ld,%ld", &robot.p.c, &robot.p.r, &robot.v.c, &robot.v.r);
        list_append(robots, &robot);
    }
    fclose(file);
}

Vec2 position(const Robot *robot, long cols, long rows, long time) {
    Vec2 p = robot->p;
    p.r = modulo(p.r + time * robot->v.r, rows);
    p.c = modulo(p.c + time * robot->v.c, cols);
    return p;
}

long quadrant(Vec2 p, long cols, long rows) {
    if (p.r < rows / 2 && p.c < cols / 2) {
        return 1;
    }
    if (p.r > rows / 2 && p.c < cols / 2) {
        return 2;
    }
    if (p.r < rows / 2 && p.c > cols / 2) {
        return 3;
    }
    if (p.r > rows / 2 && p.c > cols / 2) {
        return 4;
    }
    return 0;
}

long safety(const List *robots, long cols, long rows, long time) {
    long safety[5] = {0};
    list_for_each(item, robots) {
        Vec2 p = position(item->data, cols, rows, time);
        safety[quadrant(p, cols, rows)] += 1;
    }
    return safety[1] * safety[2] * safety[3] * safety[4];
}
