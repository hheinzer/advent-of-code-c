/*
 * Day 13: Distress Signal
 * (https://adventofcode.com/2022/day/13)
 *
 * Part 1:
 *
 * Part 2:
 */
#include "aoc.c"

typedef enum PacketDataType {
    PDT_LIST,
    PDT_INT,
} PacketDataType;

typedef struct PacketData {
    PacketDataType type;
    union {
        List *pd_list;
        int pd_int;
    };
} PacketData;

PacketData *pd_create(const char **line_ptr)
{
    // create new list
    PacketData *pd = malloc(sizeof(*pd));
    pd->type = PDT_LIST;
    pd->pd_list = list_create(sizeof(PacketData));

    const char *line = *line_ptr + 1;
    while (*line) {
        if (*line == '[') {
            // add new list
            list_push_back(pd->pd_list, pd_create(&line));

        } else if (*line == ']') {
            // end of list: return currently open list and update line pointer
            *line_ptr = line;
            return pd;

        } else if (*line == ',') {
            // comma, do nothing

        } else {
            // add integer
            PacketData *_pd = malloc(sizeof(*_pd));
            _pd->type = PDT_INT;
            sscanf(line, "%d", &_pd->pd_int);
            list_push_back(pd->pd_list, _pd);

            // in case integer had more than one digit, push line pointer forward
            while (isdigit(*(line + 1))) {
                ++line;
            }
        }
        ++line;
    }
    return pd;
}

void pd_free(PacketData *pd)
{
    // recursively free the packet
    if (pd->type == PDT_LIST) {
        for (ListElement *elem = pd->pd_list->head; elem; elem = elem->next) {
            pd_free(elem->data);
        }
        list_free(pd->pd_list);
    }
}

void pd_print(const PacketData *pd)
{
    // recursively print packet
    printf("[");
    for (ListElement *elem = pd->pd_list->head; elem; elem = elem->next) {
        const PacketData *_pd = elem->data;
        if (_pd->type == PDT_INT) {
            printf("%d%s", _pd->pd_int, (elem->next ? "," : ""));
        } else {
            pd_print(_pd);
            printf("%s", (elem->next ? "," : ""));
        }
    }
    printf("]");
}

void *pd_copy(void *_dest, const void *_src, size_t size_bytes)
{
    PacketData *dest = (PacketData *)_dest;
    PacketData *src = (PacketData *)_src;

    dest->type = src->type;

    if (src->type == PDT_INT) {
        dest->pd_int = src->pd_int;

    } else {
        // recursively copy packet
        dest->pd_list = list_create(size_bytes);
        for (const ListElement *elem = src->pd_list->head; elem; elem = elem->next) {
            const PacketData *src_data = elem->data;
            PacketData *dest_data = malloc(size_bytes);
            pd_copy(dest_data, src_data, size_bytes);
            list_push_back(dest->pd_list, dest_data);
        }
    }

    return dest;
}

int cmp_packet_asc(const void *_a, const void *_b)
{
    const PacketData *a = (PacketData *)_a;
    const PacketData *b = (PacketData *)_b;

    if ((a->type == PDT_INT) && (b->type == PDT_INT)) {
        // both are int: return comparison
        return a->pd_int - b->pd_int;

    } else if ((a->type == PDT_INT) && (b->type == PDT_LIST)) {
        // one is int, other is list: convert int to list and recompare
        PacketData a_list = {
            .type = PDT_LIST,
            .pd_list = list_create(sizeof(PacketData)),
        };
        list_push_back_copy(a_list.pd_list, a, pd_copy);
        int cmp = cmp_packet_asc(&a_list, b);
        list_free(a_list.pd_list);
        return cmp;

    } else if ((a->type == PDT_LIST) && (b->type == PDT_INT)) {
        // one is int, other is list: convert int to list and recompare
        PacketData b_list = {
            .type = PDT_LIST,
            .pd_list = list_create(sizeof(PacketData)),
        };
        list_push_back_copy(b_list.pd_list, b, pd_copy);
        int cmp = cmp_packet_asc(a, &b_list);
        list_free(b_list.pd_list);
        return cmp;

    } else {
        // both are lists: compare elements
        const ListElement *elem_a = a->pd_list->head;
        const ListElement *elem_b = b->pd_list->head;
        int cmp = 0;
        while (elem_a && elem_b && (cmp == 0)) {
            cmp = cmp_packet_asc(elem_a->data, elem_b->data);
            elem_a = elem_a->next;
            elem_b = elem_b->next;
        }

        // return comparison, or comparison of list lengths
        return (cmp ? cmp : (int)a->pd_list->nelem - (int)b->pd_list->nelem);
    }
}

int main(void)
{
    // read input
    const char **line = 0;
    const size_t n_lines = lines_read(&line, "13.txt");

    // create packets
    List *packet = list_create(sizeof(PacketData));
    for (size_t i = 0; i < n_lines; ++i) {
        if (strcmp(line[i], "")) {
            const char *l = line[i];
            PacketData *pd = pd_create(&l);
            list_push_back(packet, pd);
        }
    }

    // sum up the indices of the pair in right order
    size_t sum_right_order = 0;
    const ListElement *elem_a = packet->head;
    const ListElement *elem_b = elem_a->next;
    for (size_t i = 0; i < packet->nelem / 2; ++i) {
        int cmp = cmp_packet_asc(elem_a->data, elem_b->data);
        sum_right_order += (cmp < 0 ? i + 1 : 0);
        elem_a = elem_b->next;
        elem_b = (elem_a ? elem_a->next : 0);
    }

    // part 1
    printf("%zu\n", sum_right_order);

    // add divider packets
    PacketData *div1 = pd_create(&(const char *) { "[[2]]" });
    PacketData *div2 = pd_create(&(const char *) { "[[6]]" });
    list_push_back_copy(packet, div1, pd_copy);
    list_push_back_copy(packet, div2, pd_copy);

    // sort packets
    list_sort(packet, cmp_packet_asc);

    // part 2
    const size_t i_div1 = list_search(packet, div1, cmp_packet_asc);
    const size_t i_div2 = list_search(packet, div2, cmp_packet_asc);
    printf("%zu\n", (i_div1 + 1) * (i_div2 + 1));

    // cleanup
    lines_free(line, n_lines);
    for (ListElement *elem = packet->head; elem; elem = elem->next) {
        pd_free(elem->data);
    }
    list_free(packet);
    pd_free(div1);
    pd_free(div2);
    free(div1);
    free(div2);
}
