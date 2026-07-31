#include <stdbool.h>
#include <stdio.h>

#include "output.h"

#ifdef DEBUG
#include "logfile.h"
#endif

/*
 * Writes a uniform physical grid, not only the collocation points.  This makes
 * the resulting CSV convenient for plotting and for comparing calculations.
 */
bool write_solution(const char *filename, const problem_t *problem,
        const vfw_basis_t *basis, const solution_t *solution)
{
    FILE *file;
    int i;
    double x;
    double ratio;

    file = fopen(filename, "w");
    if (!file) {
        return false;
    }

    /* A header makes the output immediately usable by spreadsheet software. */
    fprintf(file, "x,y\n");
    for (i = 0; i < problem->output_points; i++) {
        /* Include both interval endpoints by using output_points - 1 subintervals. */
        ratio = (double)i / (double)(problem->output_points - 1);
        x = problem->x0 + (problem->x1 - problem->x0) * ratio;
        fprintf(file, "%.17g,%.17g\n", x,
                evaluate_solution(problem, basis, solution, x));
    }

    fclose(file);

#ifdef DEBUG
    if (logfile) {
        fprintf(logfile, "Solution was written to CSV file: %s.\n", filename);
    }
#endif

    return true;
}
