#include <math.h>

#include "introsort.h"

#ifdef DEBUG
#include "logfile.h"
#endif

static void quicksort(double *array, int length, int rdepth, int max_rdepth);
static void insertsort(double *array, int length);
static void heapsort(double *array, int length);
static void heapify(double *array, int length, int i);
static void swap(double *a, double *b);

/*
 * Sorts given array of said length using introsort algorithm
 */
void introsort(double *array, int length) // 560*n^2*log(n) - 142*n*log(n) + 606n - 75
{
    int max_rdepth;
#   ifdef DEBUG
    if (logfile) {
        fprintf(logfile, "The program started sorting numbers in array.\n");
    }
#   endif
    max_rdepth = 2 * log(length); // (3)
    quicksort(array, length, 1, max_rdepth); // 560*n^2*log(n) - 142*n*log(n) + 606n - 78
#   ifdef DEBUG
    if (logfile) {
        fprintf(logfile, "The program finished sorting numbers in array.\n");
    }
#   endif
}

/*
 * Sorts given array using quicksort algorithm until jump conditions are met
 * The said condition are:
 * - maximum recurtion depth being reached
 * - minimum array length being reached
 */
static void quicksort(double *array, int length, int rdepth, int max_rdepth) // 560*n^2*log(n) - 142*n*log(n) + 606n - 79
{
    int pivot_index;
    int i;

    if (length < 16) { // (1)
        /*
         * The array is short enough for insertsort to be effective
         */
        if (length <= 1) { // (1)
            /*
             * There is nothing to sort at all
             */ 
            return; // (1)
        }
        insertsort(array, length); // 10n^2 - n - 10 + 1 = 10n^2 - n - 9
        return; // (1)
    }

    if (rdepth > max_rdepth) { // (1)
        /*
         * Maximum recursion depth is reached. Quicksort may not be effective
         */
        heapsort(array, length); // // 70*n*log(n) - 17.75*log(n) + 42n - 21
        return; // (1)
    }

    /*
     * Quicksort algorithm itself
     * Dividing array in two halves around pivot element
     */
    pivot_index = length - 1; // (2)
    i = 0; // (1)
    while (i < pivot_index) { // (1) * n
        if (array[i] > array[pivot_index]) { // (3)
            swap(&array[i], &array[pivot_index]); // (12)
            swap(&array[i], &array[pivot_index - 1]); // (12)
            pivot_index--; // (2)
        } else {
            i++; // (2)
        }
    } // 29n

    /*
     * Recursion call for halves
     */
    quicksort(array, pivot_index, rdepth + 1, max_rdepth); // 280*n^2*log(n) - 71*n*log(n) + 288n^2 - 52n
    quicksort(array + pivot_index + 1, length - pivot_index - 1,
              rdepth + 1, max_rdepth); // 280*n^2*log(n) - 71*n*log(n) + 288n^2 - 32n
}

/*
 * Sorts array of said length using insertsort algorithm
 */
static void insertsort(double *array, int length) // 10n^2 - n - 10
{
    double key;
    int j;
    for (int i = 1; i < length; i++) { // (1) + (n - 1) + 2 * (n - 2) = 3n - 4
        /*
         * At the beginning of the algorithm the sorted length is 1
         */
        key = array[i]; // (2)
        j = i - 1; // (2)
        while (j >= 0 && array[j] > key) { // (5n)
            /*
             * Elements shift
             */
            array[j + 1] = array[j]; // (3)
            j--; // (2)
        } // 5n
        /*
         * ELement insertion into sorted array part
         */
        array[j + 1] = key; // (2)
    } // (10n + 6) * (n - 1) = 10n^2 + 6n - 10n - 6 = 10n^2 - 4n - 6
}

/*
 * Sorts array of said length using heapsort algorithm
 */
static void heapsort(double *array, int length) // 70*n*log(n) - 17.75*log(n) + 42n - 21
{
    /*
     * Building a tree
     */
    for (int i = length / 2 - 1; i >= 0; i--) { // (3) + (2) * n / 2 + (2) * (n / 2 - 1) = 1 + 2n
        heapify(array, length, i); // 36 + 35*log(n)
    } // (36 + 35*log(n)) * (n / 2) = 36n + 35*n*log(n) + 18 + 17.25*log(n)

    /*
     * Swaping biggest element with the end and fixing now broken tree
     */
    for (int i = length - 1; i > 0; i--) { // (2) + (n - 1) * 2 + (2) * (n - 2) = 4n - 4
        swap(&array[0], &array[i]); // (4) + (1) + (7) = (12)
        heapify(array, i, 0); // 36 + 35*log(n)
    } // (36 + 35*log(n)) * (n - 1) = 36n + 35*n*log(n) - 36 - 35*log(n)
}

/*
 * Builds a solid tree out of a broken one
 */
static void heapify(double *array, int length, int i) // 36 + 35*log(n)
{
    int root, left, right;

    root = i; // (1)
    left = i * 2 + 1; // (3)
    right = i * 2 + 2; // (3)

    /*
     * Looking for the biggest child
     */
    if (left < length && array[left] > array[root]) { // (6)
        root = left; // (1)
    }

    if (right < length && array[right] > array[root]) { // (6)
        root = right; // (1)
    }

    /*
     * If given root is not bigger than both of his children
     * swap them and fix tree starting from bigger child
     */
    if (root != i) { // (2)
        swap(&array[i], &array[root]); // (1) + (4) + (7) = (12)
        heapify(array, length, root); // 35*log(n) + 1
    }
}

/*
 * Swaps values of variables
 */
static void swap(double *a, double *b) // (7)
{
    double tmp;
    tmp = *a; // (2)
    *a = *b; // (3)
    *b = tmp; // (2)
}