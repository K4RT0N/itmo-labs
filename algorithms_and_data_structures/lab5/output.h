#ifndef _OUTPUT_H
#define _OUTPUT_H

#include <stdbool.h>

#include "problem.h"
#include "solver.h"
#include "vfw.h"

/* Writes the reconstructed solution on a uniform grid to a two-column CSV file. */
bool write_solution(const char *filename, const problem_t *problem,
        const vfw_basis_t *basis, const solution_t *solution);

#endif
