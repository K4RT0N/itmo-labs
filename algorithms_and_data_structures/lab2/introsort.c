#include <stdbool.h>
#include <stdlib.h>
#include <math.h>

#ifdef DEBUG
#include <stdio.h>
#endif

#include "stack_utils.h"
#include "introsort.h"
#include "stack.h"

#define MAX(A, B) ((A) > (B)) ? (A) : (B) /* 2 */

static void inner_introsort(stack_t *stack, int depth, int max_depth, bool reverse);
static void heapsort(stack_t *stack, bool reverse);
static void sort_tree(double *array, int length);
static void insertsort(stack_t *stack, bool reverse);
static void swap(double *a, double *b);

/*
 * Interface for calling inner_introsort function. Sorts array using introsort.
 */
void introsort(stack_t *stack)
{
    int max_depth;
    int length;

#   ifdef DEBUG
    fprintf(stderr, "introsort function started.\n");
#   endif

    length = get_length(stack); // (5)
    max_depth = log(length) * 2; // (3)

    inner_introsort(stack, 1, max_depth, false);

#   ifdef DEBUG
    fprintf(stderr, "introsort function ended.\n");
#   endif
}

/*
 * inner_introsort is user for incapsulation of depth calculation and direction correction
 */
static void inner_introsort(stack_t *stack, int depth, int max_depth, bool reverse) // 64 + 1160n^2 + 337n + 272log(n) + 1204n^2log(n)
{
    stack_t lesser, bigger;

    double pivot_value;
    double tmp;

    if (get_length(stack) < 2) { // 5
        return; // (1)
    }

    if (depth > max_depth) { // (1)
        /*
         * The maximum recursion depth was reached
         */
        heapsort(stack, reverse); // 70n + 28
        return; // (1)
    }
    if (get_length(stack) <= 16) { // (5)
        insertsort(stack, reverse); // 68 + 301n
        return; // (1)
    }

    init(&lesser); // (6)
    init(&bigger); // (6)

    pivot_value = pop(stack); // 21

    while (!is_empty(stack)) { // 7n
        tmp = pop(stack); // 21
        if (tmp > pivot_value) { // (1)
            push(&bigger, tmp); // 20
        } else {
            push(&lesser, tmp); // 20
        }
    } // 42n
    inner_introsort(&lesser, depth + 1, max_depth, !reverse); // 580n^2 + 96n + 136log(n) + 602n^2log(n)
    inner_introsort(&bigger, depth + 1, max_depth, !reverse); // 580n^2 + 96n + 136log(n) + 602n^2log(n)

    if (reverse) { // (1)
        while (!is_empty(&bigger)) {  // 7n
            tmp = pop(&bigger); // 22
            push(stack, tmp); // 19
        } // 41n
        push(stack, pivot_value); // 19
        while (!is_empty(&lesser)) { // 7n
            tmp = pop(&lesser); // 22
            push(stack, tmp); // 19
        } // 41n
    } else {
        while (!is_empty(&lesser)) { // 7n
            tmp = pop(&lesser); // 22
            push(stack, tmp); // 19
        } //41n
        push(stack, pivot_value); // 19
        while (!is_empty(&bigger)) { // 7n
            tmp = pop(&bigger); // 22
            push(stack, tmp); // 19
        } // 41n
    }
}

/*
 * Implements heapsort. Is used when maximum recursion depth was reached.
 */
static void heapsort(stack_t *stack, bool reverse) // 70n + 28
{
    double *tree;
    double tmp;
    int length;

    length = get_length(stack); // (5)
    tree = malloc(sizeof(double) * length); // (2)

    for (int i = 0; i < length; i++) { // 3n - 1
        tmp = pop(stack); // (21)
        tree[i] = tmp; // (2)
    } // 23n

    for (int i = 0; i < length; i++) { // 3n - 1
        swap(&tree[i], &tree[length - 1 - i]); // 14
        sort_tree(tree, length - i);  // 17n + 5
    } // 17n + 5 + 14

    if (reverse) { // (1)
        for (int i = 0; i < length; i++) { // 3n + 3
            tmp = tree[i]; // (2)
            push(stack, tmp); // 19
        } //21n
    }
}

/*
 * Sorts trees in form of arrays.
 */
static void sort_tree(double *array, int length) // 17n + 3
{
    for (int i = length / 2 - 1; i >= 0; i--) { // 1.5n + 3
        if (2*i + 1 >= length) { // (3)
            continue; // (1)
        }
        if (2*i + 2 >= length) { // (3)
            if (array[i] < array[2*i + 2]) { // (5)
                swap(&array[i], &array[2*i + 2]); // (14)
            }
            continue; // (1)
        }
	    if (array[i] > array[2*i + 1] && array[i] > array[2*i + 2]) { // (11)
            if (array[2*i + 1] > array[2*i + 2]) { // (7)
                swap(&array[2*i + 1], &array[i]); // (8)
            } else {
                swap(&array[2*i + 2], &array[i]); // (8)
            }
	    }
    } // 15.5n
}

/*
 * Implements insertsort algorithm for stack
 */
void insertsort(stack_t *stack, bool reverse) // 67 + 301n
{
    stack_t aux_stack, tmp_stack;
    double new_elem, tmp;

    init(&aux_stack); // (6)
    init(&tmp_stack); // (6)

    while (!is_empty(stack)) { // (6n)
        new_elem = pop(stack); // (21)
        while (!is_empty(&aux_stack)) { // 7n
            tmp = pop(&aux_stack); // (22)
            if (tmp > new_elem) { // (1)
                push(&tmp_stack, tmp); // 20
            } else {
                push(&aux_stack, tmp); // (20)
                break; // (1)
            }
        } // 43n
        push(&aux_stack, new_elem); // (20)
        while (!is_empty(&tmp_stack)) { // 7n
            tmp = pop(&tmp_stack); // 22
            push(&aux_stack, tmp); // 20
        } // 42n
    } // 99n + 41

    while (!is_empty(&aux_stack)) { // 7n
        tmp = pop(&aux_stack); // 22
        push(stack, tmp); // 20
    } // 42n

    if (!reverse) { // (1)
        reverse_stack(stack); // 147n + 13
    }
}

/*
 * Swaps elements
 */
static void swap(double *a, double *b) // (7)
{
    double tmp;
    tmp = *a; // (2)
    *a = *b; // (3)
    *b = tmp; // (2)
}
