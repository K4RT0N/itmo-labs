#ifndef _DEQUE_H
#define _DEQUE_H

#include <stdbool.h>

/*
 * Double-ended queue implementation based on a doubly linked list.
 *
 * The deque does not store a fixed capacity. Each successful insertion
 * allocates one element, therefore its practical limit is available memory.
 * The head pointer addresses the first element and the tail pointer addresses
 * the last one. For an empty deque both pointers must be NULL.
 */

/*
 * One linked-list element of deque.
 *
 * prev points to the element closer to head, and next points to the element
 * closer to tail. The corresponding pointer is NULL at either list border.
 */
typedef struct deque_elem_t {
    double data;
    struct deque_elem_t *prev;
    struct deque_elem_t *next;
} deque_elem_t;

/*
 * Deque descriptor.
 *
 * length is the current number of stored values. It is zero exactly when
 * head and tail are both NULL.
 */
typedef struct {
    long length;
    deque_elem_t *head;
    deque_elem_t *tail;
} deque_t;

/* Initializes deque as an empty linked list. */
void init_deque(deque_t *deque);

/* Frees every element currently owned by deque and restores empty state. */
void destroy_deque(deque_t *deque);

/* Adds value before the current head. Memory allocation may fail silently. */
void push_front(deque_t *deque, double value);

/* Adds value after the current tail. Memory allocation may fail silently. */
void push_back(deque_t *deque, double value);

/*
 * Removes and returns the first value.
 *
 * The caller must check that deque is not empty before this call.
 */
double pop_front(deque_t *deque);

/*
 * Removes and returns the last value.
 *
 * The caller must check that deque is not empty before this call.
 */
double pop_back(deque_t *deque);

/* Returns true when deque has no linked-list elements. */
bool is_empty(deque_t *deque);

#endif
