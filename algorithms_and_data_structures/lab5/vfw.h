#ifndef _VFW_H
#define _VFW_H

#include <stdbool.h>

/*
 * One-dimensional local Vieta-Fibonacci wavelet basis on the normalized
 * interval [0, 1].
 *
 * In every cell r / 2^k <= t < (r + 1) / 2^k, the basis contains shifted
 * Vieta-Fibonacci polynomials V_m(4u - 2), where u is the local coordinate.
 */
typedef struct {
    int order;
    int level;
    int cell_count;
    int basis_size;
    double *polynomial_coefficients;
    double *primitive_coefficients;
    double *full_integrals;
} vfw_basis_t;

/*
 * Constructs polynomial and integral tables for the local wavelet basis.
 * The integral tables are later used to recover y from the expansion of y'.
 */
bool init_vfw_basis(vfw_basis_t *basis, int order, int level);

/* Releases all tables allocated by init_vfw_basis. */
void destroy_vfw_basis(vfw_basis_t *basis);

/* Returns one compactly supported Vieta-Fibonacci wavelet at coordinate t. */
double evaluate_vfw(const vfw_basis_t *basis, int index, double t);

/* Returns the accumulated integral from 0 to t of one local wavelet. */
double integrate_vfw(const vfw_basis_t *basis, int index, double t);

#endif
