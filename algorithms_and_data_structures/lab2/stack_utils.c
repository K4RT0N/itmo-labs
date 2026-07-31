#include <stdbool.h>
#include <stdio.h>

#include "stack_utils.h"
#include "stack.h"

void reverse_stack(stack_t *stack) // 147n + 12
{
    stack_t tmp_stack1, tmp_stack2;
    double tmp;

    init(&tmp_stack1); // 6
    init(&tmp_stack2); // 6

    while (!is_empty(stack)) { // 7n
        tmp = pop(stack); // 21
        push(&tmp_stack1, tmp); // 20
    } // 41n

    while (!is_empty(&tmp_stack1)) { // 8n
        tmp = pop(&tmp_stack1); // 22
        push(&tmp_stack2, tmp); // 20
    } // 42n

    while (!is_empty(&tmp_stack2)) { // 8n
        tmp = pop(&tmp_stack2); // 22
        push(stack, tmp); // 19
    } // 41n
}

void print_data(stack_t *stack) // 99n + 6
{
    stack_t tmp_storage;
    double value;

    init(&tmp_storage); // (6)

    while (!is_empty(stack)) { // 7n
        value = pop(stack); // 21
        push(&tmp_storage, value); // 20
    } // 41n

    while (!is_empty(&tmp_storage)) { // 8n
        value = pop(&tmp_storage); // 22
        printf("%lf\n", value); // 1
        push(stack, value); // 20
    } // 43n
}