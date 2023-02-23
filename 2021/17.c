/*
 * Day 17: Trick Shot
 * (https://adventofcode.com/2021/day/17)
 *
 * Part 1:
 * - simulate the probe trajectory for different initial velocities
 *   - brute force:
 *     - 0 <= vx <= 200
 *     - 0 <= vy <= 200
 * - keep track of maximum height
 *
 * Part 2:
 * - simulate the probe trajectory for different initial velocities
 *   - brute force:
 *     -    0 <= vx <= 200
 *     - -200 <= vy <= 200
 * - count all hits
 */
#include "aoc.h"

typedef struct Vec {
    long x;
    long y;
} Vec;

typedef enum ProbeStatus {
    PS_MOVING,
    PS_TARGET_HIT,
    PS_TARGET_MISS,
} ProbeStatus;

ProbeStatus probe_move(Vec *pos, Vec *vel, const Vec *min, const Vec *max)
{
    pos->x += vel->x;
    pos->y += vel->y;
    vel->x -= SIGN(vel->x);
    vel->y -= 1;
    if ((min->x <= pos->x) && (pos->x <= max->x) && (min->y <= pos->y) && (pos->y <= max->y)) {
        return PS_TARGET_HIT;
    }
    if (pos->y < min->y) {
        return PS_TARGET_MISS;
    }
    return PS_MOVING;
}

long _probe_max_y(Vec *vel, const Vec *min, const Vec *max)
{
    Vec pos = { 0, 0 };
    ProbeStatus status = PS_MOVING;
    long max_pos_y = 0;
    while ((status = probe_move(&pos, vel, min, max)) == PS_MOVING) {
        max_pos_y = MAX(max_pos_y, pos.y);
    }
    if (status == PS_TARGET_HIT) {
        return max_pos_y;
    } else {
        return LONG_MIN;
    }
}

long probe_max_y(const Vec *min, const Vec *max)
{
    long max_pos_y = 0;
    for (long vel_x = 0; vel_x < 200; ++vel_x) {
        for (long vel_y = 0; vel_y < 200; ++vel_y) {
            Vec vel = { vel_x, vel_y };
            max_pos_y = MAX(max_pos_y, _probe_max_y(&vel, min, max));
        }
    }
    return max_pos_y;
}

size_t probe_hit_count(const Vec *min, const Vec *max)
{
    size_t count = 0;
    for (long vel_x = 0; vel_x < 200; ++vel_x) {
        for (long vel_y = -200; vel_y < 200; ++vel_y) {
            Vec vel = { vel_x, vel_y };
            if (_probe_max_y(&vel, min, max) >= max->y) {
                ++count;
            }
        }
    }
    return count;
}

int main(void)
{
    // read input
    const char **line = 0;
    const size_t n_lines = lines_read(&line, "2021/input/17.txt");

    // read target area
    Vec min, max;
    sscanf(line[0], "target area: x=%ld..%ld, y=%ld..%ld", &min.x, &max.x, &min.y, &max.y);

    // part 1
    printf("%ld\n", probe_max_y(&min, &max));

    // part 2
    printf("%zu\n", probe_hit_count(&min, &max));

    // cleanup
    lines_free(line, n_lines);
}
