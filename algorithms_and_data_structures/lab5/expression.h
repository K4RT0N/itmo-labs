#ifndef _EXPRESSION_H
#define _EXPRESSION_H

#include <stdbool.h>

/*
 * Evaluates an arithmetic expression containing x.
 *
 * Supported operators: +, -, *, /, ^ and parentheses.
 * Supported constants: pi, e.
 * Supported functions: sin, cos, tan, exp, log, ln, sqrt, abs.
 */
double evaluate_expression(const char *expression, double x, bool *valid);

#endif
