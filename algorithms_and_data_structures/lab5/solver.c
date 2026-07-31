#include <float.h>
#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "expression.h"
#include "solver.h"

#ifdef DEBUG
#include "logfile.h"
#endif

/*
 * Computes order Gauss-Legendre nodes on [0,1] and copies them into every
 * local cell.  The number of points then equals the number of coefficients,
 * which gives a square collocation system.
 */
static bool build_collocation_points(const vfw_basis_t *basis, double *points)
{
    const int order = basis->order;
    const int cells = basis->cell_count;
    const double pi = acos(-1.0);
    int cell;
    int i;
    int j;
    int p;
    double z;
    double previous_z;
    double polynomial;
    double previous_polynomial;
    double derivative;
    double local_points[20];

    /* First construct one local rule, then shift and scale it to every cell. */
    for (i = 0; i < (order + 1) / 2; i++) {
        z = cos(pi * ((double)i + 0.75) / ((double)order + 0.5));
        do {
            previous_z = z;
            polynomial = 1.0;
            previous_polynomial = 0.0;
            for (j = 1; j <= order; j++) {
                double temporary = polynomial;
                polynomial = ((2.0 * (double)j - 1.0) * z * polynomial -
                        ((double)j - 1.0) * previous_polynomial) / (double)j;
                previous_polynomial = temporary;
            }
            derivative = (double)order * (z * polynomial - previous_polynomial) /
                    (z * z - 1.0);
            z = previous_z - polynomial / derivative;
        } while (fabs(z - previous_z) > 8.0 * DBL_EPSILON);

        p = order - 1 - i;
        local_points[i] = 0.5 * (1.0 - z);
        local_points[p] = 0.5 * (1.0 + z);
    }

    for (cell = 0; cell < cells; cell++) {
        for (i = 0; i < order; i++) {
            points[cell * order + i] = ((double)cell + local_points[i]) /
                    (double)cells;
        }
    }

    return true;
}

/* Returns the maximum absolute entry; it is the stopping measure for Newton. */
static double vector_norm(const double *vector, int count)
{
    double norm;
    int i;

    norm = 0.0;
    for (i = 0; i < count; i++) {
        if (fabs(vector[i]) > norm) {
            norm = fabs(vector[i]);
        }
    }
    return norm;
}

/*
 * Solves A x = b in place by Gaussian elimination with partial pivoting.
 * Row exchanges select the largest available pivot and reduce loss of
 * accuracy when the current Newton matrix is poorly scaled.
 */
static bool solve_linear_system(double *matrix, double *right_side, int size)
{
    int column;
    int row;
    int pivot_row;
    int i;
    double pivot_abs;
    double candidate_abs;
    double factor;
    double temporary;

    for (column = 0; column < size; column++) {
        pivot_row = column;
        pivot_abs = fabs(matrix[(size_t)column * (size_t)size + (size_t)column]);
        for (row = column + 1; row < size; row++) {
            candidate_abs = fabs(matrix[(size_t)row * (size_t)size +
                    (size_t)column]);
            if (candidate_abs > pivot_abs) {
                pivot_abs = candidate_abs;
                pivot_row = row;
            }
        }

        /* A nearly zero pivot means that the Newton correction is unreliable. */
        if (pivot_abs < 1e-14) {
            return false;
        }

        /* Move the most stable pivot into the current diagonal position. */
        if (pivot_row != column) {
            for (i = column; i < size; i++) {
                temporary = matrix[(size_t)column * (size_t)size + (size_t)i];
                matrix[(size_t)column * (size_t)size + (size_t)i] =
                        matrix[(size_t)pivot_row * (size_t)size + (size_t)i];
                matrix[(size_t)pivot_row * (size_t)size + (size_t)i] = temporary;
            }
            temporary = right_side[column];
            right_side[column] = right_side[pivot_row];
            right_side[pivot_row] = temporary;
        }

        for (row = column + 1; row < size; row++) {
            factor = matrix[(size_t)row * (size_t)size + (size_t)column] /
                    matrix[(size_t)column * (size_t)size + (size_t)column];
            matrix[(size_t)row * (size_t)size + (size_t)column] = 0.0;
            for (i = column + 1; i < size; i++) {
                matrix[(size_t)row * (size_t)size + (size_t)i] -= factor *
                        matrix[(size_t)column * (size_t)size + (size_t)i];
            }
            right_side[row] -= factor * right_side[column];
        }
    }

    /* Recover the correction vector from the upper-triangular system. */
    for (row = size - 1; row >= 0; row--) {
        temporary = right_side[row];
        for (i = row + 1; i < size; i++) {
            temporary -= matrix[(size_t)row * (size_t)size + (size_t)i] *
                    right_side[i];
        }
        right_side[row] = temporary /
                matrix[(size_t)row * (size_t)size + (size_t)row];
    }

    return true;
}

/*
 * Assembles the collocation residual and, when requested, its exact Jacobian.
 * For coefficient c_j the derivative is
 * psi_j(t_i) - (2 a_i y_i + b_i) L I_j(t_i).
 */
static bool assemble_system(const problem_t *problem, const vfw_basis_t *basis,
        const double *points, const double *coefficients, double *residual,
        double *jacobian)
{
    const int size = basis->basis_size;
    const double length = problem->x1 - problem->x0;
    int i;
    int j;
    bool valid;
    double x;
    double y;
    double derivative;
    double a_value;
    double b_value;
    double c_value;
    double wavelet;
    double integral;

    for (i = 0; i < size; i++) {
        x = problem->x0 + length * points[i];
        /* Add the known initial value before accumulated wavelet contributions. */
        y = problem->y0;
        derivative = 0.0;

        /* Reconstruct y and y' from all wavelet coefficients at this point. */
        for (j = 0; j < size; j++) {
            wavelet = evaluate_vfw(basis, j, points[i]);
            integral = integrate_vfw(basis, j, points[i]);
            derivative += coefficients[j] * wavelet;
            y += length * coefficients[j] * integral;
        }

        /* Evaluate all three user expressions only after x has been mapped back. */
        a_value = evaluate_expression(problem->a_expression, x, &valid);
        if (!valid) {
            return false;
        }
        b_value = evaluate_expression(problem->b_expression, x, &valid);
        if (!valid) {
            return false;
        }
        c_value = evaluate_expression(problem->c_expression, x, &valid);
        if (!valid) {
            return false;
        }

        /* The residual is zero exactly when the Riccati equation holds here. */
        residual[i] = derivative - a_value * y * y - b_value * y - c_value;
        if (!isfinite(residual[i])) {
            return false;
        }

        /* The candidate-residual evaluation does not need a Jacobian matrix. */
        if (jacobian) {
            for (j = 0; j < size; j++) {
                wavelet = evaluate_vfw(basis, j, points[i]);
                integral = integrate_vfw(basis, j, points[i]);
                jacobian[(size_t)i * (size_t)size + (size_t)j] = wavelet -
                        (2.0 * a_value * y + b_value) * length * integral;
            }
        }
    }

    return true;
}

/* Initializes the result descriptor before any coefficient storage exists. */
void init_solution(solution_t *solution)
{
    solution->coefficient_count = 0;
    solution->coefficients = NULL;
    solution->iterations = 0;
    solution->residual_norm = INFINITY;
}

/* Releases the coefficient vector and restores the empty descriptor state. */
void destroy_solution(solution_t *solution)
{
    free(solution->coefficients);
    init_solution(solution);
}

/*
 * Solves the nonlinear collocation system by damped Newton iterations.
 * A full Newton step is tried first; if it does not decrease the residual,
 * backtracking repeatedly halves the step length.
 */
bool solve_riccati(const problem_t *problem, const vfw_basis_t *basis,
        solution_t *solution)
{
    const int size = basis->basis_size;
    int iteration;
    int j;
    int line_search_step;
    bool accepted;
    double residual_norm;
    double candidate_norm;
    double step_norm;
    double damping;
    double *points;
    double *residual;
    double *candidate_residual;
    double *jacobian;
    double *right_side;
    double *candidate;

    /* Allocate once: every Newton iteration reuses the same working arrays. */
    points = calloc((size_t)size, sizeof(double));
    residual = calloc((size_t)size, sizeof(double));
    candidate_residual = calloc((size_t)size, sizeof(double));
    jacobian = calloc((size_t)size * (size_t)size, sizeof(double));
    right_side = calloc((size_t)size, sizeof(double));
    candidate = calloc((size_t)size, sizeof(double));
    solution->coefficients = calloc((size_t)size, sizeof(double));

    if (!points || !residual || !candidate_residual || !jacobian || !right_side ||
            !candidate || !solution->coefficients) {
        free(points);
        free(residual);
        free(candidate_residual);
        free(jacobian);
        free(right_side);
        free(candidate);
        destroy_solution(solution);
        return false;
    }
    solution->coefficient_count = size;

    /* The point set depends only on the wavelet basis and is fixed during Newton. */
    build_collocation_points(basis, points);

#ifdef DEBUG
    if (logfile) {
        fprintf(logfile, "Newton solver started with %d wavelet coefficients.\n",
                size);
    }
#endif

    for (iteration = 0; iteration < problem->max_iterations; iteration++) {
        if (!assemble_system(problem, basis, points, solution->coefficients,
                residual, jacobian)) {
            break;
        }
        residual_norm = vector_norm(residual, size);
        solution->residual_norm = residual_norm;

        printf("Iteration %d: residual = %.12e\n", iteration, residual_norm);

        if (residual_norm <= problem->tolerance) {
            solution->iterations = iteration;
            free(points);
            free(residual);
            free(candidate_residual);
            free(jacobian);
            free(right_side);
            free(candidate);
            return true;
        }

        /* Linearize R(C + dC) approximately as R(C) + J(C)dC. */
        for (j = 0; j < size; j++) {
            right_side[j] = -residual[j];
        }
        if (!solve_linear_system(jacobian, right_side, size)) {
            break;
        }
        step_norm = vector_norm(right_side, size);

        /*
         * Backtracking makes Newton steps safer for strongly nonlinear cases.
         * A candidate is accepted only if its infinity-norm residual decreases.
         */
        damping = 1.0;
        accepted = false;
        for (line_search_step = 0; line_search_step < 12; line_search_step++) {
            for (j = 0; j < size; j++) {
                candidate[j] = solution->coefficients[j] + damping * right_side[j];
            }
            if (!assemble_system(problem, basis, points, candidate,
                    candidate_residual, NULL)) {
                damping *= 0.5;
                continue;
            }
            candidate_norm = vector_norm(candidate_residual, size);
            if (candidate_norm < residual_norm) {
                accepted = true;
                break;
            }
            damping *= 0.5;
        }
        if (!accepted) {
            break;
        }

        memcpy(solution->coefficients, candidate, (size_t)size * sizeof(double));
        solution->iterations = iteration + 1;
        solution->residual_norm = candidate_norm;

        /* Stop also when the accepted correction is negligible in relative size. */
        if (damping * step_norm <= problem->tolerance *
                (1.0 + vector_norm(solution->coefficients, size))) {
            free(points);
            free(residual);
            free(candidate_residual);
            free(jacobian);
            free(right_side);
            free(candidate);
            return candidate_norm <= 10.0 * problem->tolerance;
        }
    }

#ifdef DEBUG
    if (logfile) {
        fprintf(logfile, "Newton solver stopped without required convergence.\n");
    }
#endif

    free(points);
    free(residual);
    free(candidate_residual);
    free(jacobian);
    free(right_side);
    free(candidate);
    return false;
}

/*
 * Reconstructs y(x) after convergence by integrating the local derivative
 * expansion.  Values outside the requested interval are clamped to its ends.
 */
double evaluate_solution(const problem_t *problem, const vfw_basis_t *basis,
        const solution_t *solution, double x)
{
    const double length = problem->x1 - problem->x0;
    double t;
    double y;
    int j;

    t = (x - problem->x0) / length;
    if (t < 0.0) {
        t = 0.0;
    }
    if (t > 1.0) {
        t = 1.0;
    }

    y = problem->y0;
    for (j = 0; j < solution->coefficient_count; j++) {
        y += length * solution->coefficients[j] *
                integrate_vfw(basis, j, t);
    }

    return y;
}
