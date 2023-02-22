/*
 * Day 21: Monkey Math
 * (https://adventofcode.com/2022/day/21)
 *
 * Part 1:
 * - build a dict of all the monkey jobs
 *   - either shout number,
 *   - or shout math operation
 * - recusively compute the result
 *
 * Part 2:
 * - invert the tree of monkey operations on "humn"
 * - recusively compute the result
 */
#include "aoc.h"

// a monkey either shouts a number or a math operation
typedef enum Jobtype {
    JOB_NUM,
    JOB_OP
} Jobtype;
typedef struct Monkeyjob {
    Jobtype type;
    union {
        long number; // number
        struct {
            char left[5];
            char op;
            char right[5];
        } operation; // math operation
    };
    char parent[5];
    char side; // left ('l') or right ('r')
} Monkeyjob;

long monkey_compute(const Dict *monkey, const char *root)
{
    // recusively compute the result of root
    const Monkeyjob *job = dict_find(monkey, root)->data;
    if (job->type == JOB_NUM) {
        return job->number;
    } else {
        const long num_l = monkey_compute(monkey, job->operation.left);
        const long num_r = monkey_compute(monkey, job->operation.right);
        switch (job->operation.op) {
        case '+':
            return num_l + num_r;
            break;
        case '-':
            return num_l - num_r;
            break;
        case '*':
            return num_l * num_r;
            break;
        case '/':
            return num_l / num_r;
            break;
        default:
            assert(!"Illegal math operation encountered.");
        }
    }
}

void monkey_set_parents(const Dict *monkey, const char *root)
{
    // set parent of left and right child, and set their sides
    const Monkeyjob *job_p = dict_find(monkey, root)->data;
    if (job_p->type == JOB_OP) {
        const char *left = job_p->operation.left;
        Monkeyjob *job_l = dict_find(monkey, left)->data;
        strcpy(job_l->parent, root);
        job_l->side = 'l';
        monkey_set_parents(monkey, left);

        const char *right = job_p->operation.right;
        Monkeyjob *job_r = dict_find(monkey, right)->data;
        strcpy(job_r->parent, root);
        job_r->side = 'r';
        monkey_set_parents(monkey, right);
    }
}

void monkey_invert(const Dict *monkey, const char *root, const char *humn)
{
    // recusively invert the monkey tree
    Monkeyjob *job_h = dict_find(monkey, humn)->data;
    if (strcmp(humn, root)) {
        job_h->type = JOB_OP;

        // get parent
        const char *parent = job_h->parent;
        const Monkeyjob *job_p = dict_find(monkey, parent)->data;

        // find sibling and invert tree node
        const char *sibling = 0;
        if (job_h->side == 'l') {
            // p = h + s  ->  h = p - s
            // p = h - s  ->  h = s + p
            // p = h * s  ->  h = p / s
            // p = h / s  ->  h = s * p
            sibling = job_p->operation.right;
            switch (job_p->operation.op) {
            case '+':
                strcpy(job_h->operation.left, parent);
                job_h->operation.op = '-';
                strcpy(job_h->operation.right, sibling);
                break;
            case '-':
                strcpy(job_h->operation.left, sibling);
                job_h->operation.op = '+';
                strcpy(job_h->operation.right, parent);
                break;
            case '*':
                strcpy(job_h->operation.left, parent);
                job_h->operation.op = '/';
                strcpy(job_h->operation.right, sibling);
                break;
            case '/':
                strcpy(job_h->operation.left, sibling);
                job_h->operation.op = '*';
                strcpy(job_h->operation.right, parent);
                break;
            default:
                assert(!"Illegal math operation encountered.");
            }

        } else {
            // p = s + h  ->  h = p - s
            // p = s - h  ->  h = s - p
            // p = s * h  ->  h = p / s
            // p = s / h  ->  h = s / p
            sibling = job_p->operation.left;
            switch (job_p->operation.op) {
            case '+':
                strcpy(job_h->operation.left, parent);
                job_h->operation.op = '-';
                strcpy(job_h->operation.right, sibling);
                break;
            case '-':
                strcpy(job_h->operation.left, sibling);
                job_h->operation.op = '-';
                strcpy(job_h->operation.right, parent);
                break;
            case '*':
                strcpy(job_h->operation.left, parent);
                job_h->operation.op = '/';
                strcpy(job_h->operation.right, sibling);
                break;
            case '/':
                strcpy(job_h->operation.left, sibling);
                job_h->operation.op = '/';
                strcpy(job_h->operation.right, parent);
                break;
            default:
                assert(!"Illegal math operation encountered.");
            }
        }

        // if the parent is the root node, then make the operation
        // h = s - root
        // root will be set to zero in the next recursion,
        // semantically this is equivalent to what the problem asks:
        // root: left = right
        if (!strcmp(parent, root)) {
            strcpy(job_h->operation.left, sibling);
            job_h->operation.op = '-';
            strcpy(job_h->operation.right, root);
        }

        monkey_invert(monkey, root, parent);

    } else {
        // set root node to 0
        job_h->type = JOB_NUM;
        job_h->number = 0;
    }
}

int main(void)
{
    // read input
    const char **line = 0;
    const size_t n_lines = lines_read(&line, "2022/input/21.txt");

    // create monkeys
    Dict *monkey = dict_alloc(sizeof(Monkeyjob), 2 * n_lines);
    for (size_t i = 0; i < n_lines; ++i) {
        char inp[4][5] = { 0 };
        sscanf(line[i], "%4s: %4s %4s %4s", inp[0], inp[1], inp[2], inp[3]);
        Monkeyjob job = { 0 };
        if (strlen(inp[2]) && strlen(inp[3])) {
            job.type = JOB_OP;
            strcpy(job.operation.left, inp[1]);
            job.operation.op = inp[2][0];
            strcpy(job.operation.right, inp[3]);

        } else {
            job.type = JOB_NUM;
            job.number = strtod(inp[1], 0);
        }
        dict_insert(monkey, inp[0], memdup(&job, sizeof(job)));
    }

    // part 1
    printf("%ld\n", monkey_compute(monkey, "root"));

    // invert monkey tree
    monkey_set_parents(monkey, "root");
    monkey_invert(monkey, "root", "humn");

    // part 2
    printf("%ld\n", monkey_compute(monkey, "humn"));

    // cleanup
    lines_free(line, n_lines);
    dict_free(&monkey, free);
}
