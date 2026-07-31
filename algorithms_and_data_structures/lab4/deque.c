#include <stdlib.h>

#include "deque.h"

#ifdef DEBUG
#include "logfile.h"
#endif

/*
 * Initializes new empty deque.
 *
 * The initial state satisfies the deque invariant: length is zero and neither
 * end pointer refers to an element.
 */
void init_deque(deque_t *deque)
{
    deque->length = 0;

    /* Empty deque has no first or last linked-list element. */
    deque->head = NULL;
    deque->tail = NULL;

#ifdef DEBUG
    if (logfile) {
        fprintf(logfile, "An empty deque was initialized.\n");
    }
#endif
}

/*
 * Destroys given deque by removing every element from its head.
 *
 * pop_front frees the removed element and repairs the end pointers, so no
 * separate traversal or additional memory management is required here.
 */
void destroy_deque(deque_t *deque)
{
    /* Continue until the list contains no elements. */
    while (!is_empty(deque)) {
        pop_front(deque);
    }

#ifdef DEBUG
    if (logfile) {
        fprintf(logfile, "Deque elements were destroyed.\n");
    }
#endif
}

/*
 * Pushes new value to the front of deque.
 *
 * The new element becomes head. When the deque was empty it must also become
 * tail because one element represents both ends of the list.
 */
void push_front(deque_t *deque, double value)
{
    deque_elem_t *new_elem;

    /* Allocate a separate list element because deque has no fixed capacity. */
    new_elem = malloc(sizeof(deque_elem_t));
    if (new_elem == NULL) {
        /* Allocation failure leaves the existing deque unchanged. */
#ifdef DEBUG
        if (logfile) {
            fprintf(logfile, "Unable to allocate memory for a deque element.\n");
        }
#endif
        return;
    }

    /* Link the new element before the old head. */
    new_elem->data = value;
    new_elem->prev = NULL;
    new_elem->next = deque->head;

    if (is_empty(deque)) {
        /* The first inserted element is simultaneously the tail. */
        deque->tail = new_elem;
    } else {
        /* Old head is no longer first, therefore it needs a back link. */
        deque->head->prev = new_elem;
    }

    /* Publish the newly created first element. */
    deque->head = new_elem;

    /* Exactly one list element was added. */
    deque->length++;
}

/*
 * Pushes new value to the back of deque.
 *
 * The new element becomes tail. When the deque was empty it must also become
 * head because one element represents both ends of the list.
 */
void push_back(deque_t *deque, double value)
{
    deque_elem_t *new_elem;

    /* Allocate a separate list element because deque has no fixed capacity. */
    new_elem = malloc(sizeof(deque_elem_t));
    if (new_elem == NULL) {
        /* Allocation failure leaves the existing deque unchanged. */
#ifdef DEBUG
        if (logfile) {
            fprintf(logfile, "Unable to allocate memory for a deque element.\n");
        }
#endif
        return;
    }

    /* Link the new element after the old tail. */
    new_elem->data = value;
    new_elem->prev = deque->tail;
    new_elem->next = NULL;

    if (is_empty(deque)) {
        /* The first inserted element is simultaneously the head. */
        deque->head = new_elem;
    } else {
        /* Old tail is no longer last, therefore it needs a forward link. */
        deque->tail->next = new_elem;
    }

    /* Publish the newly created last element. */
    deque->tail = new_elem;

    /* Exactly one list element was added. */
    deque->length++;
}

/*
 * Returns the value of the first element of non-empty deque.
 *
 * WARNING!!!
 *
 * This function is only meant to be called after checking that the deque is
 * not empty. Dereferencing deque->head for an empty deque causes undefined
 * behaviour.
 */
double pop_front(deque_t *deque)
{
    deque_elem_t *old_head;
    double result;

    /* Keep the current head long enough to read its data and free it later. */
    old_head = deque->head;

    /* The successor, possibly NULL, becomes the candidate for new head. */
    deque->head = old_head->next;
    result = old_head->data;

    /* The removed list element no longer belongs to deque. */
    free(old_head);

    /* One element was removed before testing the resulting list state. */
    deque->length--;

    if (is_empty(deque)) {
        /* Removing the only element leaves neither a head nor a tail. */
        deque->head = NULL;
        deque->tail = NULL;
    } else {
        /* New head must not refer back to an already freed element. */
        deque->head->prev = NULL;
    }

    /* Return the value stored in the removed head element. */
    return result;
}

/*
 * Returns the value of the last element of non-empty deque.
 *
 * WARNING!!!
 *
 * This function is only meant to be called after checking that the deque is
 * not empty. Dereferencing deque->tail for an empty deque causes undefined
 * behaviour.
 */
double pop_back(deque_t *deque)
{
    deque_elem_t *old_tail;
    double result;

    /* Keep the current tail long enough to read its data and free it later. */
    old_tail = deque->tail;

    /* The predecessor, possibly NULL, becomes the candidate for new tail. */
    deque->tail = old_tail->prev;
    result = old_tail->data;

    /* The removed list element no longer belongs to deque. */
    free(old_tail);

    /* One element was removed before testing the resulting list state. */
    deque->length--;

    if (is_empty(deque)) {
        /* Removing the only element leaves neither a head nor a tail. */
        deque->head = NULL;
        deque->tail = NULL;
    } else {
        /* New tail must not refer forward to an already freed element. */
        deque->tail->next = NULL;
    }

    /* Return the value stored in the removed tail element. */
    return result;
}

/*
 * Tells calling program if given deque is empty.
 *
 * length is maintained by every insertion and removal, so it is sufficient to
 * test it instead of traversing the list.
 */
bool is_empty(deque_t *deque)
{
    return deque->length == 0;
}
