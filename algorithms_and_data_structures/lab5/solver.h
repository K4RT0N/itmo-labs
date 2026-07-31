#ifndef _SOLVER_H
#define _SOLVER_H

#include <stdbool.h>

#include "problem.h"
#include "vfw.h"

/*
 * State returned after Newton iterations: the coefficient vector and the
 * diagnostics describing the accepted final approximation.
 */
typedef struct {
    int coefficient_count;
    double *coefficients;
    int iterations;
    double residual_norm;
} solution_t;

/* Initializes an empty solution descriptor. */
void init_solution(solution_t *solution);

/* Releases all coefficients owned by solution. */
void destroy_solution(solution_t *solution);

/* Solves the Riccati problem by wavelet collocation and damped Newton steps. */
bool solve_riccati(const problem_t *problem, const vfw_basis_t *basis,
        solution_t *solution);

/* Reconstructs the approximate solution at physical coordinate x. */
double evaluate_solution(const problem_t *problem, const vfw_basis_t *basis,
        const solution_t *solution, double x);

#endif
