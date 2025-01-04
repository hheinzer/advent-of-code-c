#include "aoc.h"

typedef struct {
    long *level;
    long length;
} Report;

void parse(List *reports, const char *fname, Arena *arena);
int issafe1(const Report *report);
int issafe2(const Report *report, Arena arena);

int main(void) {
    Arena arena = arena_create(1 << 20);

    List report = list_create(&arena, sizeof(Report), 0);
    parse(&report, "2024/input/02.txt", &arena);

    long part1 = 0;
    for (ListItem *r = report.begin; r; r = r->next) {
        part1 += issafe1(r->data);
    }
    printf("%ld\n", part1);

    long part2 = 0;
    for (ListItem *r = report.begin; r; r = r->next) {
        part2 += issafe2(r->data, arena);
    }
    printf("%ld\n", part2);

    arena_destroy(&arena);
}

void parse(List *reports, const char *fname, Arena *arena) {
    FILE *file = fopen(fname, "r");
    assert(file);
    char line[256];
    while (fgets(line, sizeof(line), file)) {
        Report report = {0};
        const char *token = strtok(line, " ");
        while (token) {
            report.level = realloc(arena, report.level, ++report.length);
            report.level[report.length - 1] = strtol(token, 0, 0);
            token = strtok(0, " ");
        }
        list_append(reports, &report);
    }
    fclose(file);
}

int issafe1(const Report *report) {
    int all_increasing = 1;
    int all_decreasing = 1;
    int all_differ_between_1_and_3 = 1;
    for (long i = 0; i < report->length - 1; i++) {
        long diff = report->level[i + 1] - report->level[i];
        all_increasing &= diff > 0;
        all_decreasing &= diff < 0;
        all_differ_between_1_and_3 &= 1 <= labs(diff) && labs(diff) <= 3;
    }
    return (all_increasing || all_decreasing) && all_differ_between_1_and_3;
}

int issafe2(const Report *report, Arena arena) {
    if (issafe1(report)) {
        return 1;
    }
    Report modified = {0};
    modified.level = calloc(&arena, modified.level, report->length - 1);
    modified.length = report->length - 1;
    for (long skip = 0; skip < report->length; skip++) {
        for (long i = 0, j = 0; i < report->length; i++) {
            if (i != skip) {
                modified.level[j++] = report->level[i];
            }
        }
        if (issafe1(&modified)) {
            return 1;
        }
    }
    return 0;
}
