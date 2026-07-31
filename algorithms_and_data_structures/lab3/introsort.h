#ifndef _INTROSORT_H
#define _INTROSORT_H

#include "cqueue.h"

/*
 * Sorts the queue contents in ascending order.
 *
 * The implementation starts with quicksort, uses insertion sort for small
 * partitions and falls back to heapsort once the recursion-depth limit is
 * exceeded.
 */
void introsort(cqueue_t *cqueue);

#endif
