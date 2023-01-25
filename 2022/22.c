/*
 * Day 22: Monkey Map
 * (https://adventofcode.com/2022/day/22)
 *
 * Part 1:
 * - create input shape
 * - create every face by reading the input shape
 * - find neighbors by scanning through the shape
 *
 * Part 2:
 * - take a piece of paper, cut out the cube shape, fold it, get the neighbors and
 *   facing directions
 */
#include "aoc.h"

typedef enum Facing {
    R,
    D,
    L,
    U
} Facing;

typedef struct Face {
    size_t g_row; // global row of top left tile
    size_t g_col; // global column of top left tile
    char *map; // map of face: open ('.'), wall ('#')
    size_t neighbor[4]; // which neighbor to transfer to when leafing the face
    Facing facing[4]; // which side the runner will be facing on the neighbor face
} Face;

void face_create(Face face[6], int shape[6][6], size_t n, const char **line)
{
    for (size_t i = 0, iface = 0; i < 6; ++i) {
        for (size_t j = 0; j < 6; ++j) {
            if (shape[i][j]) {
                Face *f = &face[iface++];
                f->g_row = i * n;
                f->g_col = j * n;
                f->map = calloc((n + 2) * (n + 2), sizeof(*f->map));
                char(*map)[n + 2] = TENSOR(map, f->map);
                for (size_t ii = 0; ii < n; ++ii) { // read map
                    for (size_t jj = 0; jj < n; ++jj) {
                        map[1 + ii][1 + jj] = line[i * n + ii][j * n + jj];
                    }
                }
                for (size_t k = 1; k <= 6; ++k) { // set right neighbor
                    size_t jface1 = shape[i][(j + k) % 6];
                    if (jface1) {
                        f->neighbor[R] = jface1 - 1;
                        break;
                    }
                }
                for (size_t k = 1; k <= 6; ++k) { // set down neighbor
                    size_t jface1 = shape[(i + k) % 6][j];
                    if (jface1) {
                        f->neighbor[D] = jface1 - 1;
                        break;
                    }
                }
                for (size_t k = 1; k <= 6; ++k) { // set left neighbor
                    size_t jface1 = shape[i][(j - k) % 6];
                    if (jface1) {
                        f->neighbor[L] = jface1 - 1;
                        break;
                    }
                }
                for (size_t k = 1; k <= 6; ++k) { // set up neighbor
                    size_t jface1 = shape[(i - k) % 6][j];
                    if (jface1) {
                        f->neighbor[U] = jface1 - 1;
                        break;
                    }
                }
                f->facing[R] = R;
                f->facing[D] = D;
                f->facing[L] = L;
                f->facing[U] = U;
            }
        }
    }
}

void face_cube_remap(Face face[6], size_t n)
{
    // hard coded !!!
    if (n == 4) {
        Face new_face[6] = {
            [0] = { .neighbor = { 5, 3, 2, 1 }, .facing = { L, D, D, D } },
            [1] = { .neighbor = { 2, 4, 5, 0 }, .facing = { R, U, U, D } },
            [2] = { .neighbor = { 3, 4, 1, 0 }, .facing = { R, R, L, R } },
            [3] = { .neighbor = { 5, 4, 2, 0 }, .facing = { D, D, L, U } },
            [4] = { .neighbor = { 5, 1, 2, 3 }, .facing = { R, U, U, U } },
            [5] = { .neighbor = { 0, 1, 4, 3 }, .facing = { L, R, L, L } },
        };
        for (size_t i = 0; i < 6; ++i) {
            memcpy(face[i].neighbor, new_face[i].neighbor, sizeof(face[i].neighbor));
            memcpy(face[i].facing, new_face[i].facing, sizeof(face[i].facing));
        }

    } else if (n == 50) {
        Face new_face[6] = {
            [0] = { .neighbor = { 1, 2, 3, 5 }, .facing = { R, D, R, R } },
            [1] = { .neighbor = { 4, 2, 0, 5 }, .facing = { L, L, L, U } },
            [2] = { .neighbor = { 1, 4, 3, 0 }, .facing = { U, D, D, U } },
            [3] = { .neighbor = { 4, 5, 0, 2 }, .facing = { R, D, R, R } },
            [4] = { .neighbor = { 1, 5, 3, 2 }, .facing = { L, L, L, U } },
            [5] = { .neighbor = { 4, 1, 0, 3 }, .facing = { U, D, D, U } },
        };
        for (size_t i = 0; i < 6; ++i) {
            memcpy(face[i].neighbor, new_face[i].neighbor, sizeof(face[i].neighbor));
            memcpy(face[i].facing, new_face[i].facing, sizeof(face[i].facing));
        }

    } else {
        assert(!"Houston, we have a problem.");
    }
}

typedef struct Runner {
    size_t iface; // index of face that runner is on
    size_t row; // row on face
    size_t col; // col on face
    Facing facing;
} Runner;

void runner_init(Runner *runner, const Face face[6], size_t n)
{
    runner->iface = 0;
    runner->row = 1;
    char(*map)[n + 2] = TENSOR(map, face[0].map);
    for (size_t col = 1; col <= n; ++col) {
        if (map[runner->row][col] == '.') {
            runner->col = col;
            break;
        }
    }
    runner->facing = R;
}

void runner_move(Runner *runner, const Face face[6], size_t n, size_t move)
{
    for (size_t imove = 0; imove < move; ++imove) {
        const size_t iface = runner->iface;
        const char(*map)[n + 2] = TENSOR(map, face[iface].map);
        const size_t drow = (runner->facing == D) - (runner->facing == U);
        const size_t dcol = (runner->facing == R) - (runner->facing == L);
        const char tile = map[runner->row + drow][runner->col + dcol];
        if (tile == '#') { // hit solid wall
            break;

        } else if (tile == '.') { // hit open tile
            runner->row += drow;
            runner->col += dcol;

        } else { // hit end of face
            const Facing facing = runner->facing;
            const Facing facing_n = face[iface].facing[runner->facing];

            size_t row = SIZE_MAX;
            size_t col = SIZE_MAX;
            const size_t angle = (facing_n + 2 - facing) % 4;
            if (angle == 1) { // rotate 90 deg: cos=0, sin=1
                row = runner->col;
                col = n + 1 - runner->row;

            } else if (angle == 2) { // rotate 180 deg: cos=-1, sin=0
                row = n + 1 - runner->row;
                col = n + 1 - runner->col;

            } else if (angle == 3) { // rotate 270 deg: cos=0, sin=-1
                row = n + 1 - runner->col;
                col = runner->row;
            } else { // rotate 0 deg: cos=1, sin=0
                row = runner->row;
                col = runner->col;
            }

            if ((facing_n == R) || (facing_n == L)) { // flip vertical
                row = n + 1 - row;
            } else { // flip horizontal
                col = n + 1 - col;
            }

            const size_t iface_n = face[iface].neighbor[runner->facing];
            const char(*map_n)[n + 2] = TENSOR(map_n, face[iface_n].map);
            if (map_n[row][col] == '.') { // change face
                runner->iface = iface_n;
                runner->row = row;
                runner->col = col;
                runner->facing = facing_n;

            } else { // hit solid wall on other face
                break;
            }
        }
    }
}

void runner_turn(Runner *runner, char turn)
{
    switch (turn) {
    case 'R':
        runner->facing = (runner->facing + 1) % 4;
        break;
    case 'L':
        runner->facing = (runner->facing - 1) % 4;
        break;
    default:
        assert(!"Illegal turn signal encountered.");
    }
}

void runner_run(Runner *runner, const Face face[6], size_t n, const char *inst)
{
    while (1) {
        size_t move;
        char turn;
        int ret = sscanf(inst, "%zu%c", &move, &turn);
        runner_move(runner, face, n, move);
        if (ret == 1) {
            break;
        }
        runner_turn(runner, turn);
        inst = strchr(inst, turn) + 1;
    }
}

size_t runner_password(Runner *runner, const Face face[6])
{
    size_t row = face[runner->iface].g_row + runner->row;
    size_t col = face[runner->iface].g_col + runner->col;
    return 1000 * row + 4 * col + runner->facing;
}

int main(void)
{
    // read input
    const char **line = 0;
    const size_t n_lines = lines_read(&line, "2022/input/22.txt");

    // determine face side length, either 4 or 50
    const size_t n = (n_lines - 2 < 6 * 4 ? 4 : 50);

    // determine input shape
    int shape[6][6] = { 0 };
    for (size_t i = 0, iface1 = 1; i < 6; ++i) {
        for (size_t j = 0; j < 6; ++j) {
            if ((i * n < n_lines) && (j * n < strlen(line[i * n]))) {
                if ((line[i * n][j * n] == '.') || (line[i * n][j * n] == '#')) {
                    shape[i][j] = iface1++;
                }
            }
        }
    }

    // create the faces
    Face face[6] = { 0 };
    face_create(face, shape, n, line);

    // create runner
    Runner runner = { 0 };
    runner_init(&runner, face, n);

    // follow instructions
    const char *inst = line[n_lines - 1];
    runner_run(&runner, face, n, inst);

    // part 1
    printf("%zu\n", runner_password(&runner, face));

    // remap face to 3D cube
    face_cube_remap(face, n);

    // follow instructions
    runner_init(&runner, face, n);
    runner_run(&runner, face, n, inst);

    // part 2
    printf("%zu\n", runner_password(&runner, face));

    // cleanup
    lines_free(line, n_lines);
    for (size_t i = 0; i < 6; ++i) {
        free(face[i].map);
    }
}
