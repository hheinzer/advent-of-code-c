#include "aoc.h"

typedef struct {
    long index;
    long offset;
    long length;
} Block;

List create(const char *disk, Arena *arena);
List rearrange1(List *before, Arena *arena);
List rearrange2(List *before, Arena *arena);
long checksum(const List *block);

int main(void) {
    Arena arena = arena_create(3 * mega_byte);

    char *disk = string_parse("2024/input/09.txt", "\n", &arena);

    {
        Arena scratch = arena;
        List before = create(disk, &scratch);
        List after = rearrange1(&before, &scratch);
        printf("%ld\n", checksum(&after));
    }

    {
        Arena scratch = arena;
        List before = create(disk, &scratch);
        List after = rearrange2(&before, &scratch);
        printf("%ld\n", checksum(&after));
    }

    arena_destroy(&arena);
}

List create(const char *disk, Arena *arena) {
    List blocks = list_create(arena, sizeof(Block), nullptr);
    for (long i = 0, offset = 0; disk[i]; i++) {
        long length = disk[i] - '0';
        list_append(&blocks, &(Block){i % 2 == 0 ? i / 2 : -1, offset, length});
        offset += length;
    }
    return blocks;
}

ListItem *next_space(ListItem *item) {
    while (item && ((Block *)item->data)->index >= 0) {
        item = item->next;
    }
    return item;
}

ListItem *prev_file(ListItem *item) {
    while (item && ((Block *)item->data)->index < 0) {
        item = item->prev;
    }
    return item;
}

List rearrange1(List *before, Arena *arena) {
    List after = list_create(arena, sizeof(Block), nullptr);
    list_append(&after, before->begin->data);
    for (ListItem *b1 = next_space(before->begin), *b2 = prev_file(before->end); b1 && b2;) {
        Block *space = b1->data;
        Block *file = b2->data;
        if (space->offset >= file->offset) {
            break;
        }
        if (space->length >= file->length) {
            list_append(&after, &(Block){file->index, space->offset, file->length});
            space->length -= file->length;
            space->offset += file->length;
            if (space->length <= 0) {
                list_append(&after, b1->next->data);
                b1 = next_space(b1->next);
            }
            b2 = prev_file(b2->prev);
        }
        else {
            list_append(&after, &(Block){file->index, space->offset, space->length});
            file->length -= space->length;
            list_append(&after, b1->next->data);
            b1 = next_space(b1->next);
        }
    }
    return after;
}

List rearrange2(List *before, Arena *arena) {
    List after = list_create(arena, sizeof(Block), nullptr);
    list_append(&after, before->begin->data);
    for (ListItem *b2 = prev_file(before->end); b2; b2 = prev_file(b2->prev)) {
        Block *file = b2->data;
        for (ListItem *b1 = next_space(before->begin); b1; b1 = next_space(b1->next)) {
            Block *space = b1->data;
            if (space->offset >= file->offset) {
                break;
            }
            if (space->length >= file->length) {
                list_append(&after, &(Block){file->index, space->offset, file->length});
                space->length -= file->length;
                space->offset += file->length;
                goto next_file;
            }
        }
        list_append(&after, file);
    next_file:;
    }
    return after;
}

long checksum(const List *block) {
    long sum = 0;
    for (ListItem *b = block->begin; b; b = b->next) {
        Block *file = b->data;
        for (long i = 0, pos = file->offset; i < file->length; i++, pos++) {
            sum += pos * file->index;
        }
    }
    return sum;
}
