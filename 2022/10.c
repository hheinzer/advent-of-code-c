/*
 * Day 10: Cathode-Ray Tube
 * (https://adventofcode.com/2022/day/10)
 *
 * Part 1:
 * - use dX variable to keep track of the increment of X
 * - when addx instruction is read, set dX to its value
 *   - repeat line to simulate addition delay
 * - add up signal strengths it specified intervals
 *
 * Part 2:
 * - at every cycle (modulo 40), check if pixel inside of sprite
 *   - sprite is 3 pixels wide, with center at X
 *   - if so, mark pixel in CRT
 */
#include "../aoc/aoc.h"

int main(void)
{
    // read input
    const char **line = 0;
    const size_t n_lines = lines_read(&line, "10.txt");

    // simulate cathode-ray tube
    long sum_signal_strength = 0;
    char CRT[6 * 40];
    memset(CRT, ' ', sizeof(CRT));
    long X = 1;
    long dX = 0;
    long cycle = 1;
    for (size_t i = 0; i < n_lines; ++i, ++cycle) {
        // compute sum of signal strength
        if ((cycle - 20) % 40 == 0) {
            sum_signal_strength += cycle * X;
        }

        // update screen
        if ((X - 1 <= (cycle - 1) % 40) && ((cycle - 1) % 40 <= X + 1)) {
            CRT[cycle - 1] = '#';
        }

        // follow instruction
        const char *inst = line[i];
        if (inst[0] == 'a') {
            if (dX == 0) {
                sscanf(inst, "%*s %ld", &dX);
                --i; // repeat line
            } else {
                X += dX;
                dX = 0;
            }
        }
    }

    // part 1
    printf("%ld\n", sum_signal_strength);

    // part 2
    for (size_t i = 0; i < 6; ++i) {
        printf("%.*s\n", 40, &CRT[i * 40]);
    }

    // cleanup
    lines_free(line, n_lines);
}
