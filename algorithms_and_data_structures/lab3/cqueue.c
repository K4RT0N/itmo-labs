#include <stdlib.h>

#include "cqueue.h"

#ifdef DEBUG
#include "logfile.h"
#endif

/*
 * Creates a logically empty queue.  No element nodes are allocated until
 * push is called, so initialisation cannot fail for a valid structure.
 */
void init_cqueue(cqueue_t *cqueue, long capacity)
{
    cqueue->capacity = capacity;
    cqueue->length = 0;
    cqueue->head = NULL;
    cqueue->tail = NULL;

#ifdef DEBUG
    if (logfile) {
        fprintf(logfile, "Circular queue initialised. Capacity: %ld.\n",
                capacity);
    }
#endif
}

/*
 * Frees every node currently retained by the queue.  pop performs both the
 * unlinking and the node deallocation, so using it here keeps destruction
 * consistent with ordinary queue removal.
 */
void destroy_cqueue(cqueue_t *cqueue)
{
#ifdef DEBUG
    if (logfile) {
        fprintf(logfile, "Circular queue destruction started. Elements: %ld.\n",
                cqueue->length);
    }
#endif

    while (!is_empty(cqueue)) {
        pop(cqueue);
    }

#ifdef DEBUG
    if (logfile) {
        fprintf(logfile, "Circular queue destruction ended.\n");
    }
#endif
}

/*
 * Appends a new value to the tail of the queue.  If the queue was already
 * full, the newly appended element is retained while the old head is removed.
 */
void push(cqueue_t *cqueue, double value)
{
    cqueue_elem_t *old_tail;
    cqueue_elem_t *old_head;
    cqueue_elem_t *new_elem;

    /* Allocate and initialise the node before changing queue links. */
    new_elem = malloc(sizeof(cqueue_elem_t));
    if (new_elem == NULL) {
        /*
         * The historical interface has no error return value.  Preserve that
         * behaviour while recording the failed insertion in a DEBUG log.
         */
#ifdef DEBUG
        if (logfile) {
            fprintf(logfile, "Circular queue insertion failed: memory "
                    "allocation failed.\n");
        }
#endif
        return;
    }

    new_elem->data = value;
    new_elem->next = NULL;

    if (is_empty(cqueue)) {
        /* The first node is simultaneously the head and the tail. */
        cqueue->head = new_elem;
    } else {
        /*
         * A non-empty queue already has a tail.  Its next link must point to
         * the new last node before the tail pointer itself is updated.
         */
        old_tail = cqueue->tail;
        old_tail->next = new_elem;
    }

    /* In both branches above, the new node becomes the current tail. */
    cqueue->tail = new_elem;

    if (is_full(cqueue)) {
        /*
         * length still describes the pre-insertion state, so a full queue
         * must discard its old head and keep the same length afterwards.
         */
        old_head = cqueue->head;
        cqueue->head = old_head->next;
        free(old_head);
#ifdef DEBUG
        if (logfile) {
            fprintf(logfile, "Circular queue was full. The oldest value was "
                    "overwritten.\n");
        }
#endif
    } else {
        /*
         * No existing value was lost.  The new node therefore increases the
         * number of retained elements by one.
         */
        cqueue->length++;
    }
}

/*
 * Removes the head node and returns its value.
 *
 * The caller is responsible for checking the documented non-empty
 * precondition.  This preserves the simple low-level queue interface used by
 * the original program.
 */
double pop(cqueue_t *cqueue)
{
    cqueue_elem_t *old_head;
    double result;

    /*
     * Save the head data before freeing the node.  The next link becomes the
     * new head regardless of whether another node exists afterwards.
     */
    old_head = cqueue->head;
    cqueue->head = old_head->next;
    result = old_head->data;
    free(old_head);

    /* One retained node has been removed. */
    cqueue->length--;

    if (is_empty(cqueue)) {
        /*
         * A queue with no nodes must not leave tail pointing at freed memory.
         * Both end pointers therefore return to their empty-state values.
         */
        cqueue->head = NULL;
        cqueue->tail = NULL;
#ifdef DEBUG
        if (logfile) {
            fprintf(logfile, "Circular queue became empty after removal.\n");
        }
#endif
    }

    return result;
}

/* The length field is the authoritative empty-state invariant. */
bool is_empty(cqueue_t *cqueue)
{
    return cqueue->length == 0;
}

/* A push overwrites the oldest element exactly when length equals capacity. */
bool is_full(cqueue_t *cqueue)
{
    return cqueue->length == cqueue->capacity;
}
