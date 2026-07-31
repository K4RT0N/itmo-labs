#include <stdio.h>
#include <stdlib.h>

#include "args.h"
#include "messages.h"
#include "output.h"
#include "problem.h"
#include "solver.h"
#include "vfw.h"

#ifdef DEBUG
#include "logfile.h"
#endif

/*
 * Coordinates the program stages and centralizes cleanup labels.  Each label
 * releases only resources that have already been initialized, which keeps
 * error exits as safe as the successful path.
 */
int main(int argc, char **argv)
{
    args_t args;
    problem_t problem;
    vfw_basis_t basis;
    solution_t solution;
    int exit_code;

#ifdef DEBUG
    logfile = fopen("logfile", "w");
    if (logfile) {
        fprintf(logfile, "The Riccati solver started.\n");
    }
#endif

    /* Do not open files or allocate numerical structures until options are valid. */
    init_args(argc, argv, &args);
    if (!args.valid_usage) {
        usage_message();
        exit_code = EXIT_FAILURE;
        goto finish;
    }

    /* Initialize all descriptors before a function can fail partway through. */
    init_problem(&problem);
    init_solution(&solution);
    /* The three owning pointers must be NULL before init_vfw_basis is called. */
    basis.polynomial_coefficients = NULL;
    basis.primitive_coefficients = NULL;
    basis.full_integrals = NULL;

    /* Read and validate coefficients, interval, and numerical settings. */
    if (!read_problem(args.filename, &problem)) {
        fprintf(stderr, "Unable to read a valid Riccati problem from %s.\n",
                args.filename);
        exit_code = EXIT_FAILURE;
        goto destroy_problem;
    }

    /* Allocate the local Vieta-Fibonacci polynomial and integral tables. */
    if (!init_vfw_basis(&basis, problem.wavelet_order, problem.wavelet_level)) {
        fprintf(stderr, "Unable to allocate Vieta-Fibonacci wavelet basis.\n");
        exit_code = EXIT_FAILURE;
        goto destroy_problem;
    }

    /* Form the collocation system and iterate until the residual is small. */
    if (!solve_riccati(&problem, &basis, &solution)) {
        fprintf(stderr, "Newton method did not converge. Try a smaller interval, "
                "a lower wavelet level or another initial problem setup.\n");
        exit_code = EXIT_FAILURE;
        goto destroy_basis;
    }

    /* Reconstruct the converged approximation on the requested output grid. */
    if (!write_solution(args.output_filename, &problem, &basis, &solution)) {
        fprintf(stderr, "Unable to write %s.\n", args.output_filename);
        exit_code = EXIT_FAILURE;
        goto destroy_basis;
    }

    printf("Converged after %d Newton iterations.\n", solution.iterations);
    printf("Final residual norm: %.12e\n", solution.residual_norm);
    printf("Solution saved to %s.\n", args.output_filename);
    exit_code = EXIT_SUCCESS;

/* Release resources in reverse creation order on all paths below. */
destroy_basis:
    destroy_solution(&solution);
    destroy_vfw_basis(&basis);
destroy_problem:
    destroy_problem(&problem);
finish:
#ifdef DEBUG
    if (logfile) {
        fprintf(logfile, "The Riccati solver finished with exit code %d.\n",
                exit_code);
        fclose(logfile);
        logfile = NULL;
    }
#endif
    return exit_code;
}
