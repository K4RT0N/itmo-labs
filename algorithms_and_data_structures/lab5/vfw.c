#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

#include "vfw.h"

#ifdef DEBUG
#include <stdio.h>
#include "logfile.h"
#endif

/* Returns the row holding coefficients of one polynomial in local coordinate u. */
static double *polynomial_row(const vfw_basis_t *basis, int m)
{
    return basis->polynomial_coefficients + (size_t)m * (size_t)(basis->order + 1);
}

/* Returns the row holding coefficients of the corresponding antiderivative. */
static double *primitive_row(const vfw_basis_t *basis, int m)
{
    return basis->primitive_coefficients + (size_t)m * (size_t)(basis->order + 2);
}

/*
 * Evaluates a polynomial stored in ascending powers by Horner's method.
 * This avoids repeated calls to pow and is used both for a wavelet and its
 * precomputed local antiderivative.
 */
static double evaluate_polynomial(const double *coefficients, int degree,
        double value)
{
    double result;
    int i;

    result = 0.0;
    for (i = degree; i >= 0; i--) {
        result = result * value + coefficients[i];
    }
    return result;
}

/*
 * Builds all polynomial tables for the local Vieta-Fibonacci basis.
 * The global interval is divided into 2^level cells; every cell receives the
 * same set of order local polynomials, shifted and scaled at evaluation time.
 */
bool init_vfw_basis(vfw_basis_t *basis, int order, int level)
{
    int m;
    int p;
    double *current;
    double *previous;
    double *previous_previous;
    double *primitive;

    /* Save dimensions before allocation so destroy_vfw_basis can clean up safely. */
    basis->order = order;
    basis->level = level;
    basis->cell_count = 1 << level;
    basis->basis_size = order * basis->cell_count;
    basis->polynomial_coefficients = calloc((size_t)(order + 1) *
            (size_t)(order + 1), sizeof(double));
    basis->primitive_coefficients = calloc((size_t)(order + 1) *
            (size_t)(order + 2), sizeof(double));
    basis->full_integrals = calloc((size_t)(order + 1), sizeof(double));

    if (!basis->polynomial_coefficients || !basis->primitive_coefficients ||
            !basis->full_integrals) {
        destroy_vfw_basis(basis);
        return false;
    }

    /* Seed the recurrence V_0(z) = 0 and V_1(z) = 1. */
    polynomial_row(basis, 1)[0] = 1.0;

    /*
     * Form V_m(4u - 2) in powers of the local coordinate u.  Storing the
     * result in powers of u lets the program evaluate it efficiently later.
     */
    for (m = 2; m <= order; m++) {
        current = polynomial_row(basis, m);
        previous = polynomial_row(basis, m - 1);
        previous_previous = polynomial_row(basis, m - 2);

        for (p = 0; p <= m - 1; p++) {
            current[p] -= 2.0 * previous[p];
            if (p > 0) {
                current[p] += 4.0 * previous[p - 1];
            }
            current[p] -= previous_previous[p];
        }
    }

    /*
     * Precompute antiderivatives.  The solver expands y', therefore these
     * primitives are required to reconstruct y while preserving y(x0)=y0.
     */
    for (m = 1; m <= order; m++) {
        current = polynomial_row(basis, m);
        primitive = primitive_row(basis, m);
        for (p = 0; p <= m - 1; p++) {
            primitive[p + 1] = current[p] / (double)(p + 1);
        }
        basis->full_integrals[m] = evaluate_polynomial(primitive, m, 1.0);
    }

#ifdef DEBUG
    if (logfile) {
        fprintf(logfile, "Vieta-Fibonacci wavelet basis created: order=%d, "
                "level=%d, size=%d.\n", order, level, basis->basis_size);
    }
#endif

    return true;
}

/* Releases all basis tables and resets fields to a clearly empty state. */
void destroy_vfw_basis(vfw_basis_t *basis)
{
    free(basis->polynomial_coefficients);
    free(basis->primitive_coefficients);
    free(basis->full_integrals);
    basis->polynomial_coefficients = NULL;
    basis->primitive_coefficients = NULL;
    basis->full_integrals = NULL;
    basis->order = 0;
    basis->level = 0;
    basis->cell_count = 0;
    basis->basis_size = 0;
}

/*
 * Evaluates one local wavelet.  Its value is zero outside the cell determined
 * by index; this compact support is the local feature of the chosen basis.
 */
double evaluate_vfw(const vfw_basis_t *basis, int index, double t)
{
    int cell;
    int m;
    double left;
    double right;
    double local;
    double scale;

    if (index < 0 || index >= basis->basis_size || t < 0.0 || t > 1.0) {
        return 0.0;
    }

    /* Decode the global index into the local-cell number and polynomial number. */
    cell = index / basis->order;
    m = index % basis->order + 1;
    left = (double)cell / (double)basis->cell_count;
    right = (double)(cell + 1) / (double)basis->cell_count;

    if (t < left || (t >= right && cell + 1 < basis->cell_count)) {
        return 0.0;
    }

    local = (double)basis->cell_count * t - (double)cell;
    if (local < 0.0 || local > 1.0) {
        return 0.0;
    }

    scale = sqrt((double)basis->cell_count);
    return scale * evaluate_polynomial(polynomial_row(basis, m), m - 1, local);
}

/*
 * Integrates one local wavelet from 0 to t.  Earlier cells contribute their
 * complete integral, the current cell contributes a partial integral, and
 * later cells contribute zero.
 */
double integrate_vfw(const vfw_basis_t *basis, int index, double t)
{
    int cell;
    int m;
    double left;
    double right;
    double local;
    double scale;
    double *primitive;

    if (index < 0 || index >= basis->basis_size || t <= 0.0) {
        return 0.0;
    }
    if (t > 1.0) {
        t = 1.0;
    }

    cell = index / basis->order;
    m = index % basis->order + 1;
    left = (double)cell / (double)basis->cell_count;
    right = (double)(cell + 1) / (double)basis->cell_count;
    scale = sqrt((double)basis->cell_count);

    if (t <= left) {
        return 0.0;
    }
    if (t >= right) {
        return scale * basis->full_integrals[m] / (double)basis->cell_count;
    }

    local = (double)basis->cell_count * t - (double)cell;
    primitive = primitive_row(basis, m);
    return scale * evaluate_polynomial(primitive, m, local) /
            (double)basis->cell_count;
}
