/*
 * Day 19: Not Enough Minerals
 * (https://adventofcode.com/2022/day/19)
 *
 * Part 1:
 * - do cached depth first search
 * - also keep track of the global best score to eliminte states based on their potential
 *   to beat the global best
 *
 * Part 2:
 * - no adjustment needed, just increase the available time
 */
#include "aoc.h"

typedef enum Mineral {
    ORE,
    CLY,
    OBS,
    GEO,
    N
} Mineral;

typedef struct Blueprint {
    long amt[N][N]; // mineral cost for robot [i_robot][i_mineral]
    long max[N - 1]; // maximum number of bots
} Blueprint;

typedef struct Miningstate {
    long bot[N]; // number of bots
    long amt[N]; // amount of minerals
} Miningstate;

long _dfs(Blueprint *bp, long time, Miningstate s, Dict *cache, long *best)
{
    // check remaining time
    if (time == 0) {
        return s.amt[GEO];
    }

    // check cache
    char key[256] = "";
    Item *item = dict_find(cache,
        KEY(key, "%ld %ld %ld %ld %ld %ld %ld %ld %ld ",
            time,
            s.bot[ORE], s.bot[CLY], s.bot[OBS], s.bot[GEO],
            s.amt[ORE], s.amt[CLY], s.amt[OBS], s.amt[GEO]));
    if (item) {
        return *(long *)item->data;
    }

    // check if this state has the potential to beat the best state
    long pot = s.amt[GEO];
    for (long i = 0; i < time; ++i) {
        pot += s.bot[GEO] + i;
    }
    if (pot < *best) {
        return s.amt[GEO];
    }

    // compute state
    long max_geo = s.amt[GEO] + s.bot[GEO] * time;
    for (size_t ibot = 0; ibot < N; ++ibot) {
        // check if we need this robot
        if ((ibot != GEO) && (s.bot[ibot] >= bp->max[ibot])) {
            continue;
        }

        // compute how long we have to wait
        long wait = 0;
        int can_wait = 1;
        for (size_t j = 0; j < N; ++j) {
            if (bp->amt[ibot][j]) {
                if (s.bot[j] > 0) {
                    wait = MAX(wait, CEIL(bp->amt[ibot][j] - s.amt[j], s.bot[j]));
                } else {
                    can_wait = 0; // we cannot wait to build this robot
                }
            }
        }

        // if we can wait for the robot, wait, and build it
        if (can_wait) {
            assert(wait >= 0);
            long remtime = time - wait - 1;
            if (remtime <= 0) {
                continue; // no time to build robot
            }

            // create new state
            Miningstate ns = s;

            // mine minerals
            for (size_t j = 0; j < N; ++j) {
                ns.amt[j] += ns.bot[j] * (wait + 1);
            }

            // build robot
            for (size_t j = 0; j < N; ++j) {
                ns.amt[j] -= bp->amt[ibot][j];
            }
            ns.bot[ibot] += 1;

            // improve cache hit rate by tossing items we don't need
            for (size_t j = 0; j < N - 1; ++j) {
                ns.amt[j] = MIN(ns.amt[j], bp->max[j] * remtime);
            }

            max_geo = MAX(max_geo, _dfs(bp, remtime, ns, cache, best));
        }
    }

    // cache max_geo
    dict_insert(cache, key, memdup(&max_geo, sizeof(max_geo)));

    // update global best
    *best = MAX(*best, max_geo);

    return max_geo;
}

long dfs(Blueprint *bp, long time)
{
    Miningstate s = { 0 };
    s.bot[ORE] = 1;
    Dict *cache = dict_alloc(sizeof(long), 200000);
    long best = 0;
    long max_geo = _dfs(bp, time, s, cache, &best);
    dict_free(&cache, free);
    return max_geo;
}

int main(void)
{
    // read input
    const char **line = 0;
    const size_t n_lines = lines_read(&line, "2022/input/19.txt");

    // create blueprints
    Blueprint *bp = calloc(n_lines, sizeof(*bp));
    for (size_t i = 0; i < n_lines; ++i) {
        sscanf(line[i],
            "Blueprint %*d: "
            "Each ore robot costs %ld ore. "
            "Each clay robot costs %ld ore. "
            "Each obsidian robot costs %ld ore and %ld clay. "
            "Each geode robot costs %ld ore and %ld obsidian.",
            &bp[i].amt[ORE][ORE],
            &bp[i].amt[CLY][ORE],
            &bp[i].amt[OBS][ORE], &bp[i].amt[OBS][CLY],
            &bp[i].amt[GEO][ORE], &bp[i].amt[GEO][OBS]);
        for (size_t j = 0; j < N - 1; ++j) {
            for (size_t k = 0; k < N; ++k) {
                bp[i].max[j] = MAX(bp[i].max[j], bp[i].amt[k][j]);
            }
        }
    }

    // part 1
    long sum_quality_level = 0;
    for (size_t i = 0; i < n_lines; ++i) {
        sum_quality_level += (i + 1) * dfs(&bp[i], 24);
    }
    printf("%ld\n", sum_quality_level);

    // part 2
    long prod_geo_cnt = 1;
    for (size_t i = 0; i < MIN(n_lines, 3); ++i) {
        prod_geo_cnt *= dfs(&bp[i], 32);
    }
    printf("%ld\n", prod_geo_cnt);

    // cleanup
    lines_free(line, n_lines);
    free(bp);
}
