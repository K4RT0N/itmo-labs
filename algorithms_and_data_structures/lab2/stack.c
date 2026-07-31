#include "stack.h"

#include <stddef.h>
#include <stdlib.h>

void init(stack_t *stack) // (4)
{
    stack->head = NULL; // (3)
    stack->length = 0; // (3)
}

void erase(stack_t *stack) // (6n + 9)
{
    selem_t *head, *new_head;
    head = stack->head; // (3)

    while (head != NULL) { // (n)
        new_head = head->next; // (3)
        free(head); // (1)
        head = new_head; // (1)
    } // (5n)

    stack->head = NULL; // (3)
    stack->length = 0; // (3)
}

void push(stack_t *stack, double value) // (18)
{
    selem_t *new_head;

    new_head = malloc(sizeof(selem_t)); // (2)
    new_head->value = value; // (3)
    new_head->next = stack->head; // (5)
    stack->head = new_head; // (3)
    stack->length++; // (5)
}

double pop(stack_t *stack) // (19)
{
    selem_t *old_head, *new_head;
    double return_value;

    old_head = stack->head; // (3)
    new_head = old_head->next; // (3)
    stack->head = new_head; // (3)
    stack->length--; // (5)

    return_value = old_head->value; // (3)
    free(old_head); // (1)

    return return_value; // (1)
}

bool is_empty(stack_t *stack) // (4)
{
    return stack->length == 0; // (4)
}

int get_length(stack_t *stack) // (3)
{
    return stack->length; // (3)
}