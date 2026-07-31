#ifndef _CQUEUE_H
#define _CQUEUE_H

#include <stdbool.h>

/*
 * A single node of the linked circular-queue representation.
 *
 * The implementation links nodes in insertion order.  head points to the
 * oldest retained element, tail points to the most recently inserted one.
 */
typedef struct cqueue_elem_t {
    double data;
    struct cqueue_elem_t *next;
} cqueue_elem_t;

/*
 * Fixed-capacity queue used by the sorting task.
 *
 * Once length reaches capacity, push keeps the newest value by removing the
 * previous head element.  The queue owns every allocated cqueue_elem_t node.
 */
typedef struct {
    long capacity;
    long length;
    cqueue_elem_t *head;
    cqueue_elem_t *tail;
} cqueue_t;

/* Initialises an empty queue whose capacity is supplied by the user. */
void init_cqueue(cqueue_t *cqueue, long capacity);

/* Frees all currently retained nodes and leaves the queue empty. */
void destroy_cqueue(cqueue_t *cqueue);

/* Appends a value; when full, discards the oldest value first. */
void push(cqueue_t *cqueue, double value);

/*
 * Removes and returns the oldest value.
 * Precondition: is_empty(cqueue) is false.
 */
double pop(cqueue_t *cqueue);

/* Reports whether the queue currently contains no nodes. */
bool is_empty(cqueue_t *cqueue);

/* Reports whether the next push will overwrite the oldest node. */
bool is_full(cqueue_t *cqueue);

#endif
