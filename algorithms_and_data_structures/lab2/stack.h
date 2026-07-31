#ifndef STACK_H
#define STACK_H

#include <stdbool.h>

typedef struct stack_t {
    struct selem_t *head;
    int length;
} stack_t;

typedef struct selem_t {
    struct selem_t *next;
    double value;
} selem_t;

void init(stack_t *stack);
void erase(stack_t *stack);

void push(stack_t *stack, double value);
double pop(stack_t *stack);

bool is_empty(stack_t *stack);
int get_length(stack_t *stack);

#endif