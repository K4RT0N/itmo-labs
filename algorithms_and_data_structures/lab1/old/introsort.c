#include <math.h> 

#ifdef DEBUG
#include <stdio.h>
#endif

#include "introsort.h"

#define MAX(X,Y) (((X) > (Y)) ? (X) : (Y)) /* (1) */

static void inner_introsort(double *array, int length,
                            int current_depth, int max_depth);
static void insertion_sort(double *array, int length);
static void sift_down(double *array, int start, int end);
static void heapsort(double *array, int length);
static void swap(double *a, double *b);

/*
 * Interface for calling inner_introsort function. Sorts array using introsort.
 */
void introsort(double *array, int length)
{
    int max_depth;

#   ifdef DEBUG
    fprintf(stderr, "introsort function started.\n");
#   endif

    max_depth = log(length) * 2;

    inner_introsort(array, length, 1, max_depth);

#   ifdef DEBUG
    fprintf(stderr, "introsort function ended.\n");
#   endif
}

/*
 * inner_introsort is user for incapsulation of depth calculation.
 */
static void inner_introsort(double *array, int length,
                            int current_depth, int max_depth)
{
    int pivot_index, i;
    
    if (length < 16) { // (1)
        // In the worse case scenario is never actually reached
        if (length < 2) { // (1)
            /*
             * Nothing to sort. Returning.
             */
            return; // (1)
        }
        /*
         * The array is short enoung to consider insertion sort effective.
         */
        insertion_sort(array, length); // (4439)
        return;
    }
    
    if (current_depth > max_depth) { // (1)
        /*
         * The maximum recursion depth is reached. Switch to heapsort.
         */
        heapsort(array, length); // 82*n*log(n) + 82*log(n) + 13.5n - 15
        return;
    }

    pivot_index = length - 1; // (2)
    i = 0; // (1)
    
    /*
     * Sorting algoritms itself. 
     */
    while (i != pivot_index - 1) { // (2) * (n - 2)
        if (array[i] > array[pivot_index]) { // (3)
            swap(&array[pivot_index], &array[pivot_index - 1]); // (1) + (7) + (5)
            swap(&array[i], &array[pivot_index]); // (1) + (7) + 4
            pivot_index = pivot_index - 1; // (2)
        } else {
            i = i + 1; // (2)
        }
    }

    if (array[pivot_index - 1] > array[pivot_index]) { // (4)
        swap(&array[pivot_index - 1], &array[pivot_index]); // (1) + (7) + (5)
        pivot_index = pivot_index - 1; // (2)
    }

    /*
     * Recursive sort for halfs to the left and to the right of the pivot.
     */
    // Up to this point this function in the worst case scenario executes
    // 51 + 2n elementary commands. The depth of recursion is limited by 2log(n),
    // thus algorithm complexity will never reach quadratic polinomial value, since
    // each layer of the recursion tree can be represented as an + b
    // elementary operations.
    inner_introsort(array, pivot_index, current_depth + 1, max_depth);
    inner_introsort(array + pivot_index + 1, length - pivot_index - 1,
                    current_depth + 1, max_depth);
    // As the result, the complexity in the worst case scenario provided by call of
    // heapsort, which uses 82*k*log(k) + 82*log(k) + 13.5k - 15 elementary operations.
    // (k is size of data slise in the leaf of recursion t
    // Using logarithm arithmetic qualities we can surely say, that the sum of elementary
    // operations executed by heapsort on the heafs of recursion tree will not be bigger
    // than 82*n*log(n) + 82*log(n) * 13.5n - 15.
    // (n is full data size).
    // Thus, the sorting algoriths has upper complexity of O(n log(n))
}

/*
 * Implements insertion sort. Good for short arrays.
 */
static void insertion_sort(double *array, int length) // (47 + 1440 + 2472 + 480) = (4439)
// In the worst case scenario the length is 16, so we'll use it
{
    int x;
    for (int i = 1; i < length; i++) { // (1), (1) * 16, (1) * 15
        x = i; // (1) * 15
        while (x > 0 && array[x] < array[x - 1]) { // (6) * 16 (worst case scenario) * 15 = (1440)
            swap(&array[x], &array[x - 1]); // ((1) + (7) + (5)) * 16 * 15= (13) * 16 * 15 = (2472)
            x = x - 1; // (2) * 15 * 16 = (480)
        }
    }
}

/*
 * Sorts trees in form of arrays.
 */
static void sift_down(double *array, int start, int end) // (1) + (41) * log(n)
{
   int root = start; // (1)

    while (2 * root + 1 <= end) { // (3) * log(n)
        int child = 2 * root + 1; // (3)
        int swap_i = root; // (1)

        if (array[swap_i] < array[child]) { // (3)
            swap_i = child; // (1)
        }

        if (child + 1 <= end && array[swap_i] < array[child + 1]) { // (7)
            swap_i = child + 1; // (2)
        }

        if (swap_i == root) { // (1)
            return; // (1)
        }

        swap(&array[root], &array[swap_i]); // (1) + (7) + (4)
        root = swap_i; // (1)
    } // (38) * log(n)
}

/*
 * Implements heapsort. Is used when maximum recursion depth was reached.
 */
static void heapsort(double *array, int length) // 82*n*log(n) + 82*log(n) + 13.5n - 15
{
    for (int i = length / 2 - 1; i >= 0; i--) { // (3), (n / 2), 2 * (n / 2 - 1)
        sift_down(array, i, length - 1); // (1) + (1) + (41) * log(n) + 1 = (3) + (41) * log(n)
    } // ((3) + (41) * log(n)) * (n / 2 - 1) = 
      // 3*n/2 - 3 + 41*n*log(n)/2 - 41log(n)

    for (int end = length - 1; end > 0; end--) { // (2), (n), 2 * (n - 1)
        swap(&array[0], &array[end]); // (1) + (7) + (4) = (12)
        sift_down(array, 0, end - 1); // (1) + (1) + (41) * log(n) + 1 = (3) = (41) * log(n)
    } // (n-1) * (12 + 41 * log(n)) = 12n + 41*n*log(n) - 12 - 41*log(n)
}

/*
 * Swaps elements
 */
static void swap(double *a, double *b) // (7)
{
    double tmp;
    tmp = *a; // (2)
    *a = *b; // (3)
    *b = tmp; // (2)
}
