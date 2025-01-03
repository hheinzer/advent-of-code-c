#include "aoc.h"

typedef struct {
    long index;
    long offset;
    long length;
} Block;

List create(const char *diskmap, Arena *arena);
List rearrange1(List *before, Arena *arena);
List rearrange2(List *before, Arena *arena);
long checksum(const List *blocks);

int main(void) {
    Arena arena = arena_create(1 << 21);

    char *diskmap = parse_string("2024/input/09.txt", "\n", &arena);

    {
        Arena scratch = arena;
        List before = create(diskmap, &scratch);
        List after = rearrange1(&before, &scratch);
        printf("%ld\n", checksum(&after));
    }

    {
        Arena scratch = arena;
        List before = create(diskmap, &scratch);
        List after = rearrange2(&before, &scratch);
        printf("%ld\n", checksum(&after));
    }

    arena_destroy(&arena);
}

List create(const char *diskmap, Arena *arena) {
    List blocks = list_create(arena, sizeof(Block), 0);
    for (long i = 0, offset = 0; diskmap[i]; i++) {
        long length = diskmap[i] - '0';
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
    List after = list_create(arena, sizeof(Block), 0);
    list_append(&after, before->begin->data);
    for (ListItem *item1 = next_space(before->begin), *item2 = prev_file(before->end);
         item1 && item2;) {
        Block *s = item1->data;
        Block *f = item2->data;
        if (s->offset >= f->offset) {
            break;
        }
        if (s->length >= f->length) {
            list_append(&after, &(Block){f->index, s->offset, f->length});
            s->length -= f->length;
            s->offset += f->length;
            if (s->length <= 0) {
                list_append(&after, item1->next->data);
                item1 = next_space(item1->next);
            }
            item2 = prev_file(item2->prev);
        }
        else {
            list_append(&after, &(Block){f->index, s->offset, s->length});
            f->length -= s->length;
            list_append(&after, item1->next->data);
            item1 = next_space(item1->next);
        }
    }
    return after;
}

List rearrange2(List *before, Arena *arena) {
    List after = list_create(arena, sizeof(Block), 0);
    list_append(&after, before->begin->data);
    for (ListItem *item2 = prev_file(before->end); item2; item2 = prev_file(item2->prev)) {
        Block *f = item2->data;
        for (ListItem *item1 = next_space(before->begin); item1; item1 = next_space(item1->next)) {
            Block *s = item1->data;
            if (s->offset >= f->offset) {
                break;
            }
            if (s->length >= f->length) {
                list_append(&after, &(Block){f->index, s->offset, f->length});
                s->length -= f->length;
                s->offset += f->length;
                goto next_file;
            }
        }
        list_append(&after, f);
    next_file:
        continue;
    }
    return after;
}

long checksum(const List *blocks) {
    long sum = 0;
    list_for_each(item, blocks) {
        Block *block = item->data;
        for (long i = 0, pos = block->offset; i < block->length; i++, pos++) {
            sum += pos * block->index;
        }
    }
    return sum;
}
