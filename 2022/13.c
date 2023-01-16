/*
 * Day 13: Distress Signal
 * (https://adventofcode.com/2022/day/13)
 *
 * Part 1:
 * - create a new data type that can either be a list or an integer
 * - create all the packets with this data type
 * - create a comparison function for this data data type and check if the packets are in
 *   order
 *
 * Part 2:
 * - add divider packets
 * - use comparison function to sort list
 * - search for dividers in sorted list
 */
#include "aoc.h"

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
    pd->pd_list = list_alloc(sizeof(PacketData));

    const char *line = *line_ptr + 1;
    while (*line) {
        if (*line == '[') {
            // add new list
            list_insert_last(pd->pd_list, pd_create(&line));

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
            list_insert_last(pd->pd_list, _pd);

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
        Node *node = pd->pd_list->first;
        while (node) {
            pd_free(node->data);
            node->data = 0;
            node = node->next;
        }
        list_free(&pd->pd_list, free);
    }
    free(pd);
}

void pd_print(const PacketData *pd)
{
    // recursively print packet
    printf("[");
    Node *node = pd->pd_list->first;
    while (node) {
        const PacketData *_pd = node->data;
        if (_pd->type == PDT_INT) {
            printf("%d%s", _pd->pd_int, (node->next ? "," : ""));
        } else {
            pd_print(_pd);
            printf("%s", (node->next ? "," : ""));
        }
        node = node->next;
    }
    printf("]");
}

PacketData *pd_copy(const PacketData *src)
{
    PacketData *dest = malloc(sizeof(*dest));

    dest->type = src->type;

    if (src->type == PDT_INT) {
        dest->pd_int = src->pd_int;

    } else {
        // recursively copy packet
        dest->pd_list = list_alloc(sizeof(PacketData));
        const Node *node = src->pd_list->first;
        while (node) {
            list_insert_last(dest->pd_list, pd_copy(node->data));
            node = node->next;
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
            .pd_list = list_alloc(sizeof(PacketData)),
        };
        list_insert_last(a_list.pd_list, pd_copy(a));
        int cmp = cmp_packet_asc(&a_list, b);
        list_free(&a_list.pd_list, free);
        return cmp;

    } else if ((a->type == PDT_LIST) && (b->type == PDT_INT)) {
        // one is int, other is list: convert int to list and recompare
        PacketData b_list = {
            .type = PDT_LIST,
            .pd_list = list_alloc(sizeof(PacketData)),
        };
        list_insert_last(b_list.pd_list, pd_copy(b));
        int cmp = cmp_packet_asc(a, &b_list);
        list_free(&b_list.pd_list, free);
        return cmp;

    } else {
        // both are lists: compare packets
        const Node *node_a = a->pd_list->first;
        const Node *node_b = b->pd_list->first;
        int cmp = 0;
        while (node_a && node_b && (cmp == 0)) {
            cmp = cmp_packet_asc(node_a->data, node_b->data);
            node_a = node_a->next;
            node_b = node_b->next;
        }

        // return comparison, or comparison of list lengths
        return (cmp ? cmp : (int)a->pd_list->len - (int)b->pd_list->len);
    }
}

int main(void)
{
    // read input
    const char **line = 0;
    const size_t n_lines = lines_read(&line, "2022/input/13.txt");

    // create packets
    List *packet = list_alloc(sizeof(PacketData));
    for (size_t i = 0; i < n_lines; ++i) {
        if (strcmp(line[i], "")) {
            const char *l = line[i];
            PacketData *pd = pd_create(&l);
            list_insert_last(packet, pd);
        }
    }

    // sum up the indices of the pair in right order
    size_t sum_right_order = 0;
    const Node *node_a = packet->first;
    const Node *node_b = node_a->next;
    for (size_t i = 0; i < packet->len / 2; ++i) {
        int cmp = cmp_packet_asc(node_a->data, node_b->data);
        sum_right_order += (cmp < 0 ? i + 1 : 0);
        node_a = node_b->next;
        node_b = (node_a ? node_a->next : 0);
    }

    // part 1
    printf("%zu\n", sum_right_order);

    // add divider packets
    PacketData *div1 = pd_create(&(const char *) { "[[2]]" });
    PacketData *div2 = pd_create(&(const char *) { "[[6]]" });
    list_insert_last(packet, pd_copy(div1));
    list_insert_last(packet, pd_copy(div2));

    // sort packets
    list_sort(packet, cmp_packet_asc);

    // part 2
    const size_t i_div1 = list_index(packet, div1, cmp_packet_asc);
    const size_t i_div2 = list_index(packet, div2, cmp_packet_asc);
    printf("%zu\n", (i_div1 + 1) * (i_div2 + 1));

    // cleanup
    lines_free(line, n_lines);
    list_free(&packet, pd_free);
    pd_free(div1);
    pd_free(div2);
}
