#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#include "solvemethods.h"
#include "data.h"

void find_roots(double **outer_roots, int *outer_count);

int main(void) {
    double *roots;
    int count;

    find_roots(&roots, &count);
    
    if(!count) {
        printf("Couldn't find any roots\n");
        return 0;
    } else if(count == -1) {
        printf("Every number is a root.\n");
        return 0;
    }
    for(int i = 0; i < count; i++) {
        printf("x_%d = %lf\n", i + 1, roots[i]);
    }

    free(roots);

    return 0;
}

void find_roots(double **outer_roots, int *outer_count) {
    double coeffs[7];
    double *roots;
    int count;
    bool res;

    roots = (double *) malloc(sizeof(double) * 7);
    count = 0;

    get_coeffs(coeffs);
    
    /*
     * a != 0 ? 
     */
    if(coeffs[0] != 0) {
        newton_method(coeffs, roots, &count, &res);
        if(!res) {
            *outer_roots = roots;
            *outer_count = count;
            return;
        }
        horner(coeffs, roots[count-1]);
    }

    /*
     * b != 0 ?
     */
    if(coeffs[1] != 0) {
        chord_method(coeffs, roots, &count, &res);
        if(!res) {
            *outer_roots = roots;
            *outer_count = count;
            return;
        }
        filter(coeffs, &count);
        horner(coeffs, roots[count-1]);
    }

    /*
     * c != 0 ?
     */
    if(coeffs[2] != 0) {
        chord_newton_method(coeffs, roots, &count, &res);
        if(!res) {
            *outer_roots = roots;
            *outer_count = count;
            return;
        }
        filter(coeffs, &count);
        horner(coeffs, roots[count-1]);
    }

    /*
     * d != 0 ?
     */
    if(coeffs[3] != 0) {
        cardano_method(coeffs, roots, &count);
        *outer_roots = roots;
        *outer_count = count;
        filter(coeffs, &count);
        return;
    }

    /*
     * k != 0 ?
     */
    if(coeffs[4] != 0) {
        discriminant(coeffs, roots, &count);
        *outer_roots = roots;
        *outer_count = count;
        filter(coeffs, &count);
        return;
    }

    /*
     * n != 0 ?
     */
    if(coeffs[5] != 0) {
        linear(coeffs, roots, &count);
        *outer_roots = roots;
        *outer_count = count;
        filter(coeffs, &count);
        return;
    }

    /*
     * m == 0 ?
     */
    if(coeffs[6] == 0) {
#       ifdef DEBUG
        fprintf(stderr, "No roots.\n");
#       endif
        count = -1;
        *outer_roots = roots;
        *outer_count = count;
        filter(coeffs, &count);
        return;
    }

    *outer_roots = roots;
    *outer_count = count;
    filter(coeffs, &count);
}
