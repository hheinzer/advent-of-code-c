/*
 * Day 7: No Space Left On Device
 * (https://adventofcode.com/2022/day/7)
 *
 * Part 1:
 * - use a stack to keep track of where we are in the file system
 *   - for every "$ cd <somewhere>" we add a new directory
 *   - for every "$ cd .." we remove to top directory
 * - every file that we encounter is (directly or indirectly) part of all the folders in
 *   the path
 *   - add its size to all directories in path
 * - sort final directory size array in ascending order
 * - add up all sizes until a size is larger than 100000
 *
 * Part 2:
 * - compute the minimum space that we need to free
 * - find the smallest folder that is at least as big
 */
#include "aoc.h"

CMP(size_t)

int main(void)
{
    // read input
    const char **line = 0;
    const size_t n_lines = lines_read(&line, "2022/input/07.txt");

    // find all directory sizes
    size_t *size_dir = 0;
    size_t n_dirs = 0;
    Stack *path = stack_alloc(sizeof(size_t));
    for (size_t i = 0; i < n_lines; ++i) {
        if (!strcmp(line[i], "$ cd ..")) {
            // go up a directory, pop top directory from path
            free(stack_pop(path));

        } else if (!strncmp(line[i], "$ cd", 4)) {
            // go down a directory, add directory to path
            stack_push(path, COPY(n_dirs));
            size_dir = realloc(size_dir, ++n_dirs * sizeof(*size_dir));
            size_dir[n_dirs - 1] = 0;

        } else if (!strncmp(line[i], "$ ls", 4)) {
            // list directory, do nothing

        } else {
            // get file size (if file) and add it to all directories in path
            if (strncmp(line[i], "dir", 3)) {
                size_t size = 0;
                sscanf(line[i], "%zu %*s", &size);
                for (const Node *node = path->first; node; node = node->next) {
                    size_dir[*(size_t *)node->data] += size;
                }
            }
        }
    }

    // part 1
    qsort(size_dir, n_dirs, sizeof(*size_dir), cmp_size_t_asc);
    size_t total_size = 0;
    for (size_t i = 0; i < n_dirs; ++i) {
        if (size_dir[i] <= 100000) {
            total_size += size_dir[i];
        } else {
            break;
        }
    }
    printf("%zu\n", total_size);

    // compute the space that we need to free
    const size_t total_space = 70000000;
    const size_t needed_space = 30000000;
    const size_t used_space = size_dir[n_dirs - 1];
    const size_t free_space = total_space - used_space;
    const size_t min_delete_space = needed_space - free_space;

    // part 2
    for (size_t i = 0; i < n_dirs; ++i) {
        if (size_dir[i] >= min_delete_space) {
            printf("%zu\n", size_dir[i]);
            break;
        }
    }

    // cleanup
    lines_free(line, n_lines);
    free(size_dir);
    stack_free(&path, free);
}
