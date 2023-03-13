/*
 * Day 20: Trench Map
 * (https://adventofcode.com/2021/day/20)
 *
 * Part 1:
 * - create the input image with 2 pixels per side padding
 * - enhance the image, growing it by one pixel every time
 *   - keep track of the outside pixels, for the input, the ouside pixels flip every turn
 * - count the lit pixels
 *
 * Part 2:
 * - repeat
 */
#include "aoc.h"

char *image_enhance(char *image, size_t *ni, size_t *nj, char *outside, const char *enhance)
{
    // enhance all outside pixels
    if (*outside == '.') {
        *outside = enhance[strtoul("000000000", 0, 2)];
    } else {
        *outside = enhance[strtoul("111111111", 0, 2)];
    }

    // create new image, +1 in all directions
    *ni += 2;
    *nj += 2;
    char *image_ = calloc((*ni) * (*nj), sizeof(*image_));
    memset(image_, '.', (*ni) * (*nj) * sizeof(*image_));

    // enhance image
    for (size_t i = 0; i < *ni; ++i) {
        for (size_t j = 0; j < *nj; ++j) {
            if ((2 <= i) && (i < *ni - 2) && (2 <= j) && (j < *nj - 2)) {
                char mod[10] = "000000000";
                mod[0] = (image[(i - 1 - 1) * (*nj - 2) + (j - 1 - 1)] == '#' ? '1' : '0');
                mod[1] = (image[(i - 1 - 1) * (*nj - 2) + (j - 1 + 0)] == '#' ? '1' : '0');
                mod[2] = (image[(i - 1 - 1) * (*nj - 2) + (j - 1 + 1)] == '#' ? '1' : '0');
                mod[3] = (image[(i - 1 + 0) * (*nj - 2) + (j - 1 - 1)] == '#' ? '1' : '0');
                mod[4] = (image[(i - 1 + 0) * (*nj - 2) + (j - 1 + 0)] == '#' ? '1' : '0');
                mod[5] = (image[(i - 1 + 0) * (*nj - 2) + (j - 1 + 1)] == '#' ? '1' : '0');
                mod[6] = (image[(i - 1 + 1) * (*nj - 2) + (j - 1 - 1)] == '#' ? '1' : '0');
                mod[7] = (image[(i - 1 + 1) * (*nj - 2) + (j - 1 + 0)] == '#' ? '1' : '0');
                mod[8] = (image[(i - 1 + 1) * (*nj - 2) + (j - 1 + 1)] == '#' ? '1' : '0');
                image_[i * (*nj) + j] = enhance[strtoul(mod, 0, 2)];
            } else {
                image_[i * (*nj) + j] = *outside;
            }
        }
    }

    // exchange images
    free(image);
    return image_;
}

size_t image_count_pixels(const char *image, size_t ni, size_t nj)
{
    size_t count = 0;
    for (size_t i = 0; i < ni; ++i) {
        for (size_t j = 0; j < nj; ++j) {
            if (image[i * nj + j] == '#') ++count;
        }
    }
    return count;
}

int main(void)
{
    // read input
    const char **line = 0;
    const size_t n_lines = lines_read(&line, "2021/input/20.txt");

    // get image enhancement algorithm
    const char *enhance = line[0];

    // create input image (with padding)
    size_t ni = n_lines - 2 + 4;
    size_t nj = strlen(line[n_lines - 1]) + 4;
    char *image = calloc(ni * nj, sizeof(*image));
    memset(image, '.', ni * nj * sizeof(*image));
    for (size_t i = 2; i < ni - 2; ++i) {
        for (size_t j = 2; j < nj - 2; ++j) {
            image[i * nj + j] = line[i][j - 2];
        }
    }

    // enhance image
    char outside = '.';
    for (size_t e = 0; e < 2; ++e) {
        image = image_enhance(image, &ni, &nj, &outside, enhance);
    }

    // part 1
    printf("%zu\n", image_count_pixels(image, ni, nj));

    // enhance image more
    for (size_t e = 2; e < 50; ++e) {
        image = image_enhance(image, &ni, &nj, &outside, enhance);
    }

    // part 2
    printf("%zu\n", image_count_pixels(image, ni, nj));

    // cleanup
    lines_free(line, n_lines);
    free(image);
}
