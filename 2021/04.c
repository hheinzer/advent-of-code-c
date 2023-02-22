/*
 * Day 4: Giant Squid
 * (https://adventofcode.com/2021/day/4)
 *
 * Part 1:
 * - play bingo
 * - find the first winning board
 *
 * Part 2:
 * - find the last winning board
 */
#include "aoc.h"

void board_mark_number(long board[5][5], long num)
{
    for (size_t i = 0; i < 5; ++i) {
        for (size_t j = 0; j < 5; ++j) {
            if (board[i][j] == num) {
                board[i][j] *= -1;
            }
        }
    }
}

int board_is_winner(long board[5][5])
{
    for (size_t i = 0; i < 5; ++i) {
        int row = 1;
        int col = 1;
        for (size_t j = 0; j < 5; ++j) {
            row &= (board[i][j] < 0);
            col &= (board[j][i] < 0);
        }
        if (row || col) {
            return 1;
        }
    }
    return 0;
}

long board_sum_unmakred(long board[5][5])
{
    long sum = 0;
    for (size_t i = 0; i < 5; ++i) {
        for (size_t j = 0; j < 5; ++j) {
            if (board[i][j] >= 0) {
                sum += board[i][j];
            }
        }
    }
    return sum;
}

void play_bingo(long *score, size_t *rank, long (*board)[5][5], size_t nb,
    const char *_numbers)
{
    char *numbers = strdup(_numbers);
    char *tok = strtok(numbers, ",");
    size_t irank = 1;
    while (tok) {
        const long num = strtol(tok, 0, 10);
        for (size_t i = 0; i < nb; ++i) {
            board_mark_number(board[i], num);
            if (!score[i] && board_is_winner(board[i])) {
                score[i] = board_sum_unmakred(board[i]) * num;
                rank[i] = irank++;
            }
        }
        tok = strtok(0, ",");
    }
    free(numbers);
}

int main(void)
{
    // read input
    const char **line = 0;
    const size_t n_lines = lines_read(&line, "2021/input/04.txt");

    // create boards
    const size_t nb = (n_lines - 1) / 6;
    long(*board)[5][5] = calloc(nb, sizeof(*board));
    for (size_t i = 2, ib = 0; i < n_lines; i += 6, ++ib) {
        for (size_t j = 0; j < 5; ++j) {
            sscanf(line[i + j], "%ld %ld %ld %ld %ld",
                &board[ib][j][0], &board[ib][j][1], &board[ib][j][2],
                &board[ib][j][3], &board[ib][j][4]);
        }
    }

    // play bingo
    long *score = calloc(nb, sizeof(*score));
    size_t *rank = calloc(nb, sizeof(*rank));
    play_bingo(score, rank, board, nb, line[0]);

    // part 1
    for (size_t i = 0; i < nb; ++i) {
        if (rank[i] == 1) {
            printf("%ld\n", score[i]);
        }
    }

    // part 2
    for (size_t i = 0; i < nb; ++i) {
        if (rank[i] == nb) {
            printf("%ld\n", score[i]);
        }
    }

    // cleanup
    lines_free(line, n_lines);
    free(board);
    free(score);
    free(rank);
}
