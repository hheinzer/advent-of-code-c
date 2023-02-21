/*
 * Day 16: Packet Decoder
 * (https://adventofcode.com/2021/day/16)
 *
 * Part 1:
 * - parse the packets according to the packet format
 * - instead of using actual binary, use a string, makes it easier to go through in order
 * - traverse packet and all sub packets and compute the version number
 *
 * Part 2:
 * - traverse packet and all sub packets and evaluate them
 */
#include "aoc.h"

char *hex_to_bin(const char *hex)
{
    static const char map[][5] = {
        ['0'] = "0000",
        ['1'] = "0001",
        ['2'] = "0010",
        ['3'] = "0011",
        ['4'] = "0100",
        ['5'] = "0101",
        ['6'] = "0110",
        ['7'] = "0111",
        ['8'] = "1000",
        ['9'] = "1001",
        ['A'] = "1010",
        ['B'] = "1011",
        ['C'] = "1100",
        ['D'] = "1101",
        ['E'] = "1110",
        ['F'] = "1111",
    };
    const size_t n = strlen(hex);
    char *bin = calloc(4 * n + 1, sizeof(*bin));
    for (size_t i = 0; i < n; ++i) {
        strcat(bin, map[(size_t)hex[i]]);
    }
    return bin;
}

typedef enum PacketType {
    PT_SUM,
    PT_PRODUCT,
    PT_MINIMUM,
    PT_MAXIMUM,
    PT_LITERAL,
    PT_GREATER,
    PT_LESS,
    PT_EQUAL,
} PacketType;

typedef struct Packet {
    size_t version;
    PacketType type;
    union {
        size_t number;
        struct {
            size_t n_sub;
            struct Packet **sub;
        } operator;
    };
} Packet;

size_t bin_to_size_t(const char **input, size_t n)
{
    char *buf = calloc(n + 1, sizeof(*buf));
    strncpy(buf, *input, n);
    size_t ret = strtoul(buf, 0, 2);
    *input += n; // advance input pointer
    free(buf);
    return ret;
}

Packet *packet_alloc(const char **input)
{
    Packet *packet = calloc(1, sizeof(*packet));
    packet->version = bin_to_size_t(input, 3);
    packet->type = bin_to_size_t(input, 3);
    if (packet->type == PT_LITERAL) {
        char *buf = calloc(strlen(*input) + 1, sizeof(*buf));
        do {
            strncat(buf, (*input) + 1, 4);
            *input += 5;
        } while (*((*input) - 5) == '1');
        packet->number = strtoul(buf, 0, 2);
        free(buf);
    } else {
        const int length_type = bin_to_size_t(input, 1);
        if (length_type == 0) { // read fixed bit width sub packets
            const size_t total_length = bin_to_size_t(input, 15);
            size_t *n_sub = &packet->operator.n_sub;
            const char *input_prev;
            for (size_t i = 0; i < total_length; i += (*input) - input_prev) {
                packet->operator.sub = realloc(packet->operator.sub,
                    ++(*n_sub) * sizeof(*packet->operator.sub)); // NOLINT
                input_prev = *input;
                packet->operator.sub[(*n_sub) - 1] = packet_alloc(input);
            }
        } else { // read fixed count sub packets
            assert(length_type == 1);
            packet->operator.n_sub = bin_to_size_t(input, 11);
            packet->operator.sub = calloc(packet->operator.n_sub, sizeof(Packet *));
            assert(packet->operator.sub);
            for (size_t i = 0; i < packet->operator.n_sub; ++i) {
                packet->operator.sub[i] = packet_alloc(input);
                assert(packet->operator.sub[i]);
            }
        }
    }
    return packet;
}

size_t packet_sum_version(const Packet *packet)
{
    size_t sum = packet->version;
    if (packet->type != PT_LITERAL) {
        for (size_t i = 0; i < packet->operator.n_sub; ++i) {
            sum += packet_sum_version(packet->operator.sub[i]);
        }
    }
    return sum;
}

size_t packet_eval(const Packet *packet)
{
    size_t ret = 0;
    switch (packet->type) {
    case PT_SUM:
        for (size_t i = 0; i < packet->operator.n_sub; ++i) {
            ret += packet_eval(packet->operator.sub[i]);
        }
        break;
    case PT_PRODUCT:
        ret = 1;
        for (size_t i = 0; i < packet->operator.n_sub; ++i) {
            ret *= packet_eval(packet->operator.sub[i]);
        }
        break;
    case PT_MINIMUM:
        ret = SIZE_MAX;
        for (size_t i = 0; i < packet->operator.n_sub; ++i) {
            ret = MIN(ret, packet_eval(packet->operator.sub[i]));
        }
        break;
    case PT_MAXIMUM:
        for (size_t i = 0; i < packet->operator.n_sub; ++i) {
            ret = MAX(ret, packet_eval(packet->operator.sub[i]));
        }
        break;
    case PT_LITERAL:
        ret = packet->number;
        break;
    case PT_GREATER:
        assert(packet->operator.n_sub == 2);
        ret = (packet_eval(packet->operator.sub[0]) > packet_eval(packet->operator.sub[1]));
        break;
    case PT_LESS:
        assert(packet->operator.n_sub == 2);
        ret = (packet_eval(packet->operator.sub[0]) < packet_eval(packet->operator.sub[1]));
        break;
    case PT_EQUAL:
        assert(packet->operator.n_sub == 2);
        ret = (packet_eval(packet->operator.sub[0]) == packet_eval(packet->operator.sub[1]));
        break;
    default:
        assert(!"Illegal packet type encountered.");
    }
    return ret;
}

void packet_free(Packet *packet)
{
    if (packet->type != PT_LITERAL) {
        for (size_t i = 0; i < packet->operator.n_sub; ++i) {
            packet_free(packet->operator.sub[i]);
        }
        free(packet->operator.sub);
    }
    free(packet);
}

int main(void)
{
    // read input
    const char **line = 0;
    const size_t n_lines = lines_read(&line, "2021/input/16.txt");

    // convert hex to binary
    char *input_ptr = hex_to_bin(line[0]);

    // create packet (and sub packets)
    const char *input = input_ptr;
    Packet *packet = packet_alloc(&input);

    // part 1
    printf("%zu\n", packet_sum_version(packet));

    // part 2
    printf("%zu\n", packet_eval(packet));

    // cleanup
    lines_free(line, n_lines);
    free(input_ptr);
    packet_free(packet);
}
