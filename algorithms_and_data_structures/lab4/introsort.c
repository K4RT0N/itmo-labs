#include <stdlib.h>
#include <math.h>

#include "introsort.h"
#include "deque.h"

#ifdef DEBUG
#include "logfile.h"
#endif

static void quicksort(deque_t *deque, int rdepth, int max_rdepth);
static void insertsort(deque_t *deque);
static void heapsort(deque_t *deque);

void introsort(deque_t *deque)
{
    int max_rdepth;

#ifdef DEBUG
    if (logfile) {
        fprintf(logfile, "The program started sorting %ld numbers in deque.\n",
                deque->length);
    }
#endif

    /* Calculating maximum recurtion depth */
    max_rdepth = 2 * log(deque->length);
    
    quicksort(deque, 1, max_rdepth);

#ifdef DEBUG
    if (logfile) {
        fprintf(logfile, "The program finished sorting numbers in deque.\n");
    }
#endif
}

static void quicksort(deque_t *deque, int rdepth, int max_rdepth)
{
    double pivot_value;
    double tmp;

    deque_t be_deque;
    deque_t l_deque;

    if (deque->length <= 16) {
        /* The deque length is little enough */
        if (deque->length < 2) {
            /* There is nothing to sort */
            return;
        }
        /* Jumping to insertion sort algorithm */
#ifdef DEBUG
        if (logfile) {
            fprintf(logfile, "Introsort selected insertion sort for %ld numbers.\n",
                    deque->length);
        }
#endif
        insertsort(deque);
        return;
    }
    if (rdepth > max_rdepth) {
        /* Maximum recursion depth has been reached.
         * Jumping to heapsort algorithm*/
#ifdef DEBUG
        if (logfile) {
            fprintf(logfile, "Introsort selected heapsort after reaching "
                    "maximum recursion depth.\n");
        }
#endif
        heapsort(deque);
        return;
    }

    /* quicksort algorithm */

    /* Taking head element as pivot */
    pivot_value = pop_front(deque);

    /* Initiating halves */
    init_deque(&be_deque);
    init_deque(&l_deque);
    while(!is_empty(deque)) {
        /* Dividing initial deque into two */
        tmp = pop_front(deque);
        if (tmp >= pivot_value) {
            /* Number belongs to be_deque */
            push_back(&be_deque, tmp);
        } else {
            /* Number belongs to l_deque */
            push_back(&l_deque, tmp);
        }
    }

    /* Recursive call */
    quicksort(&be_deque, rdepth + 1, max_rdepth);
    quicksort(&l_deque, rdepth + 1, max_rdepth);

    /* deque recovery */
    while (!is_empty(&l_deque)) {
        /* Putting back elements bigger or equal to pivot */
        tmp = pop_front(&l_deque);
        push_back(deque, tmp);
    }
    /* Putting back pivot */
    push_back(deque, pivot_value);
    while (!is_empty(&be_deque)) {
        /* Putting back elements lesser than pivot */
        tmp = pop_front(&be_deque);
        push_back(deque, tmp);
    }

    /* Freeing halves */
    destroy_deque(&be_deque);
    destroy_deque(&l_deque);
}

/* This function implements insertsort for deque */
static void insertsort(deque_t *deque)
{
    deque_t sorted_deque;
    deque_t tmp_deque;
    double new_elem;
    double tmp;

    bool inserted;

    /* Initializing helping deques */
    init_deque(&sorted_deque);
    init_deque(&tmp_deque);

    /* Addign first element into sorted_deque */
    tmp = pop_front(deque);
    push_back(&sorted_deque, tmp);

    /* The alglrithm itself */
    while (!is_empty(deque)) {
        /* Taking new element from deque */
        new_elem = pop_front(deque);
        inserted = false;
        while (!is_empty(&sorted_deque) && !inserted) {
            /* Taking next element in sorted_deque*/
            tmp = pop_front(&sorted_deque);
            if (tmp > new_elem) {
                /* Inserting new element at it's place */
                push_back(&tmp_deque, new_elem);
                inserted = true;
            }
            /* Putting next element into tmp_deque */
            push_back(&tmp_deque, tmp);
        }
        if (!inserted) {
            /* Pugging new element into tmp_deque if it was not before */
            push_back(&tmp_deque, new_elem);
        }
        while (!is_empty(&sorted_deque)) {
            /* Pugging the rest of sorted_deque into tmp_deque */
            tmp = pop_front(&sorted_deque);
            push_back(&tmp_deque, tmp);
        }
        while (!is_empty(&tmp_deque)) {
            /* Puttign the content of tmp_deque into sorted_deque */
            tmp = pop_front(&tmp_deque);
            push_back(&sorted_deque, tmp);
        }
    }

    while (!is_empty(&sorted_deque)) {
        /* Restoring the content of deque */
        tmp = pop_front(&sorted_deque);
        push_back(deque, tmp);
    }

    /* Freeing memory */
    destroy_deque(&sorted_deque);
    destroy_deque(&tmp_deque);
}

/* This function implements heapsort for deque */
static void heapsort(deque_t *deque)
{
    double *heap_array;
    int heap_size;
    int i;
    double tmp;

    /* Creating array representation of the deque */
    heap_size = deque->length;
    heap_array = (double*)malloc(heap_size * sizeof(double));
    if (heap_array == NULL) {
#ifdef DEBUG
        if (logfile) {
            fprintf(logfile, "Unable to allocate memory for heapsort array.\n");
        }
#endif
        return;  /* Memory allocation failed */
    }

    /* Copying elements from deque to array */
    for (i = 0; i < heap_size; i++) {
        heap_array[i] = pop_front(deque);
    }

    /* Building max-heap using bottom-up approach */
    for (i = heap_size / 2 - 1; i >= 0; i--) {
        int root = i;
        while (root < heap_size) {
            int left = 2 * root + 1;
            int right = 2 * root + 2;
            int largest = root;

            /* Finding the largest value */
            if (left < heap_size && heap_array[left] > heap_array[largest]) {
                largest = left;
            }
            if (right < heap_size && heap_array[right] > heap_array[largest]) {
                largest = right;
            }

            if (largest == root) {
                break;  /* Heap property satisfied */
            }

            /* Swapping with the largest child */
            tmp = heap_array[root];
            heap_array[root] = heap_array[largest];
            heap_array[largest] = tmp;
            root = largest;
        }
    }

    /* Sorting by moving maximum elements to the end */
    for (i = heap_size - 1; i > 0; i--) {
        int root;
        int current_size;

        /* Root is maximum, swap with last element */
        tmp = heap_array[0];
        heap_array[0] = heap_array[i];
        heap_array[i] = tmp;

        /* Restoring heap property */
        root = 0;
        current_size = i;
        while (root < current_size) {
            int left = 2 * root + 1;
            int right = 2 * root + 2;
            int largest = root;

            /* Finding the largest value */
            if (left < current_size && heap_array[left] > heap_array[largest]) {
                largest = left;
            }
            if (right < current_size && heap_array[right] > heap_array[largest]) {
                largest = right;
            }

            if (largest == root) {
                break;  /* Heap property satisfied */
            }

            /* Swapping with the largest child */
            tmp = heap_array[root];
            heap_array[root] = heap_array[largest];
            heap_array[largest] = tmp;
            root = largest;
        }
    }

    /* Restoring sorted content back to original deque */
    for (i = 0; i < heap_size; i++) {
        push_back(deque, heap_array[i]);
    }

    /* Freeing memory */
    free(heap_array);
}
