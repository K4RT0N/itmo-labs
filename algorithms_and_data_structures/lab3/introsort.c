#include <stdlib.h>
#include <math.h>
#include <stdio.h>

#include "introsort.h"
#include "cqueue.h"

#ifdef DEBUG
#include "logfile.h"
#endif

/*
 * Quicksort partitions a queue around its head element.  rdepth tracks the
 * current recursion level, while max_rdepth decides when the safe heapsort
 * fallback is required.
 */
static void quicksort(cqueue_t *cqueue, int rdepth, int max_rdepth);

/* Sorts a small queue by repeatedly inserting into a temporary sorted queue. */
static void insertsort(cqueue_t *cqueue);

/* Sorts a queue through a temporary max-heap stored in a contiguous array. */
static void heapsort(cqueue_t *cqueue);

/*
 * Selects the initial sorting strategy.  A queue of zero or one element is
 * already sorted and must bypass log(0), which is not a valid depth limit.
 */
void introsort(cqueue_t *cqueue)
{
    int max_rdepth;

#ifdef DEBUG
    if (logfile) {
        fprintf(logfile, "Introsort started. Queue length: %ld.\n",
                cqueue->length);
    }
#endif

    if (cqueue->length < 2) {
#ifdef DEBUG
        if (logfile) {
            fprintf(logfile, "Introsort ended: no sorting is needed.\n");
        }
#endif
        return;
    }

    /*
     * The depth limit preserves the original introsort design: quicksort is
     * allowed initially, but an unexpectedly deep recursion switches to the
     * guaranteed O(n log n) heapsort path.
     */
    max_rdepth = (int)(2 * log((double)cqueue->length));
    quicksort(cqueue, 1, max_rdepth);

#ifdef DEBUG
    if (logfile) {
        fprintf(logfile, "Introsort ended.\n");
    }
#endif
}

/*
 * Recursively sorts one queue partition.  The queue itself is rebuilt in
 * ascending order before the function returns to its caller.
 */
static void quicksort(cqueue_t *cqueue, int rdepth, int max_rdepth)
{
    double pivot_value;
    double tmp;

    cqueue_t be_cqueue;
    cqueue_t l_cqueue;
    long be_counter;
    long l_counter;
    long i;

    if (cqueue->length <= 16) {
        /*
         * Small partitions are cheaper to sort by insertion than by creating
         * another recursive quicksort level.
         */
        if (cqueue->length < 2) {
#ifdef DEBUG
            if (logfile) {
                fprintf(logfile, "Quicksort depth %d reached a trivial "
                        "partition.\n", rdepth);
            }
#endif
            return;
        }

#ifdef DEBUG
        if (logfile) {
            fprintf(logfile, "Quicksort depth %d switched to insertion sort "
                    "for %ld elements.\n", rdepth, cqueue->length);
        }
#endif
        insertsort(cqueue);
        return;
    }

    if (rdepth > max_rdepth) {
        /*
         * A deeper-than-expected recursion may signal an unfavourable pivot
         * sequence, so heapsort replaces further quicksort partitioning.
         */
#ifdef DEBUG
        if (logfile) {
            fprintf(logfile, "Quicksort depth limit exceeded. Heapsort is "
                    "used for %ld elements.\n", cqueue->length);
        }
#endif
        heapsort(cqueue);
        return;
    }

    /* The original algorithm uses the current head value as the pivot. */
    pivot_value = pop(cqueue);

    /*
     * First count the two partitions while preserving their original queue
     * order.  Each popped value is appended immediately back to cqueue.
     */
    be_counter = 0;
    l_counter = 0;
    for (i = 0; i < cqueue->length; i++) {
        tmp = pop(cqueue);
        if (tmp >= pivot_value) {
            /* Values not smaller than the pivot form the right partition. */
            be_counter++;
        } else {
            /* Strictly smaller values form the left partition. */
            l_counter++;
        }
        push(cqueue, tmp);
    }

#ifdef DEBUG
    if (logfile) {
        fprintf(logfile, "Quicksort depth %d selected pivot %lf. Left: %ld, "
                "right: %ld.\n", rdepth, pivot_value, l_counter, be_counter);
    }
#endif

    /*
     * The counted sizes become capacities for temporary queues, preventing
     * their fixed-capacity push operation from discarding partition values.
     */
    init_cqueue(&be_cqueue, be_counter);
    init_cqueue(&l_cqueue, l_counter);

    /*
     * Move every value into the appropriate partition.  The source queue is
     * intentionally emptied before recursive calls sort the two halves.
     */
    while (!is_empty(cqueue)) {
        tmp = pop(cqueue);
        if (tmp >= pivot_value) {
            push(&be_cqueue, tmp);
        } else {
            push(&l_cqueue, tmp);
        }
    }

    /* Sorting both halves yields ascending values on either side of pivot. */
    quicksort(&be_cqueue, rdepth + 1, max_rdepth);
    quicksort(&l_cqueue, rdepth + 1, max_rdepth);

    /*
     * Rebuild cqueue in ascending order: all smaller values, the pivot, then
     * all values greater than or equal to the pivot.
     */
    while (!is_empty(&l_cqueue)) {
        tmp = pop(&l_cqueue);
        push(cqueue, tmp);
    }

    push(cqueue, pivot_value);

    while (!is_empty(&be_cqueue)) {
        tmp = pop(&be_cqueue);
        push(cqueue, tmp);
    }

    /* The temporary queues are empty here, but destruction preserves API use. */
    destroy_cqueue(&be_cqueue);
    destroy_cqueue(&l_cqueue);
}

/*
 * Performs insertion sort using two helper queues:
 * sorted_cqueue holds the ordered prefix and tmp_cqueue assembles the next
 * ordered prefix before it replaces sorted_cqueue.
 */
static void insertsort(cqueue_t *cqueue)
{
    cqueue_t sorted_cqueue;
    cqueue_t tmp_cqueue;
    double new_elem;
    double tmp;

    bool inserted;

    /*
     * Both helpers can temporarily contain every element of cqueue, so their
     * capacities match the partition size at entry.
     */
    init_cqueue(&sorted_cqueue, cqueue->length);
    init_cqueue(&tmp_cqueue, cqueue->length);

    /* Seed the sorted prefix with the first value from the input partition. */
    tmp = pop(cqueue);
    push(&sorted_cqueue, tmp);

    while (!is_empty(cqueue)) {
        new_elem = pop(cqueue);
        inserted = false;

        /*
         * Transfer the ordered prefix to tmp_cqueue.  The first value greater
         * than new_elem marks the insertion position.
         */
        while (!is_empty(&sorted_cqueue) && !inserted) {
            tmp = pop(&sorted_cqueue);
            if (tmp > new_elem) {
                push(&tmp_cqueue, new_elem);
                inserted = true;
            }
            push(&tmp_cqueue, tmp);
        }

        if (!inserted) {
            /*
             * No larger prefix value existed, so the new value belongs at the
             * end of the ordered sequence.
             */
            push(&tmp_cqueue, new_elem);
        }

        /*
         * Remaining prefix values are larger than the inserted element and
         * therefore follow it unchanged.
         */
        while (!is_empty(&sorted_cqueue)) {
            tmp = pop(&sorted_cqueue);
            push(&tmp_cqueue, tmp);
        }

        /* tmp_cqueue now holds the new prefix and becomes sorted_cqueue. */
        while (!is_empty(&tmp_cqueue)) {
            tmp = pop(&tmp_cqueue);
            push(&sorted_cqueue, tmp);
        }
    }

    /* Return the completed ascending sequence to the caller's queue. */
    while (!is_empty(&sorted_cqueue)) {
        tmp = pop(&sorted_cqueue);
        push(cqueue, tmp);
    }

    destroy_cqueue(&sorted_cqueue);
    destroy_cqueue(&tmp_cqueue);
}

/*
 * Sorts the current queue through a max-heap.
 *
 * Unlike the former helper-queue implementation, the final ascending array
 * is written directly back to cqueue.  This avoids creating a helper queue
 * with zero capacity after the input queue has been emptied.
 */
static void heapsort(cqueue_t *cqueue)
{
    double *heap_array;
    long heap_size;
    long i;
    double tmp;

    heap_size = cqueue->length;

    /*
     * Allocate before removing queue elements.  On allocation failure the
     * original queue remains intact, which is safer than returning a partial
     * sort after values have already been popped.
     */
    heap_array = malloc((size_t)heap_size * sizeof(double));
    if (heap_array == NULL) {
#ifdef DEBUG
        if (logfile) {
            fprintf(logfile, "Heapsort allocation failed. The partition was "
                    "left unchanged.\n");
        }
#endif
        return;
    }

#ifdef DEBUG
    if (logfile) {
        fprintf(logfile, "Heapsort started for %ld elements.\n", heap_size);
    }
#endif

    /* Copy all queue values into the array, leaving cqueue empty for output. */
    for (i = 0; i < heap_size; i++) {
        heap_array[i] = pop(cqueue);
    }

    /*
     * Build a max-heap bottom-up.  Every parent is sifted down until both
     * children are no larger than it.
     */
    for (i = heap_size / 2 - 1; i >= 0; i--) {
        long root;
        long left;
        long right;
        long largest;
        double swap_value;

        root = i;
        while (1) {
            left = 2 * root + 1;
            right = 2 * root + 2;
            largest = root;

            if (left < heap_size && heap_array[left] > heap_array[largest]) {
                largest = left;
            }

            if (right < heap_size && heap_array[right] > heap_array[largest]) {
                largest = right;
            }

            if (largest == root) {
                /* The subtree rooted here already satisfies heap order. */
                break;
            }

            swap_value = heap_array[root];
            heap_array[root] = heap_array[largest];
            heap_array[largest] = swap_value;
            root = largest;
        }
    }

    /*
     * Repeatedly move the largest root to the final unused array position,
     * then restore max-heap order in the reduced prefix.
     */
    for (i = heap_size - 1; i > 0; i--) {
        long root;
        long current_size;
        long left;
        long right;
        long largest;
        double swap_value;

        swap_value = heap_array[0];
        heap_array[0] = heap_array[i];
        heap_array[i] = swap_value;

        root = 0;
        current_size = i;
        while (1) {
            left = 2 * root + 1;
            right = 2 * root + 2;
            largest = root;

            if (left < current_size
                    && heap_array[left] > heap_array[largest]) {
                largest = left;
            }

            if (right < current_size
                    && heap_array[right] > heap_array[largest]) {
                largest = right;
            }

            if (largest == root) {
                break;
            }

            swap_value = heap_array[root];
            heap_array[root] = heap_array[largest];
            heap_array[largest] = swap_value;
            root = largest;
        }
    }

    /*
     * Standard max-heap extraction leaves the array in ascending order.  The
     * queue is empty and still has its original capacity, so no value is
     * overwritten while it is rebuilt.
     */
    for (i = 0; i < heap_size; i++) {
        tmp = heap_array[i];
        push(cqueue, tmp);
    }

    free(heap_array);

#ifdef DEBUG
    if (logfile) {
        fprintf(logfile, "Heapsort ended.\n");
    }
#endif
}
