#ifndef _PROBLEM_H
#define _PROBLEM_H

#include <stdbool.h>

/*
 * Input data for one Riccati initial-value problem.
 *
 * The coefficients are stored as textual expressions and are evaluated only
 * when the nonlinear residual is assembled at a specific point.
 */
typedef struct {
    char *a_expression;
    char *b_expression;
    char *c_expression;
    double x0;
    double x1;
    double y0;
    int wavelet_order;
    int wavelet_level;
    double tolerance;
    int max_iterations;
    int output_points;
} problem_t;

/* Initializes all fields with safe defaults before file parsing begins. */
void init_problem(problem_t *problem);

/* Reads and validates one key=value problem description file. */
bool read_problem(const char *filename, problem_t *problem);

/* Releases strings allocated while reading the problem description. */
void destroy_problem(problem_t *problem);

#endif
