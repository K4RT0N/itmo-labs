#define _POSIX_C_SOURCE 200809L

#include <ctype.h>
#include <errno.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "expression.h"
#include "problem.h"

#ifdef DEBUG
#include "logfile.h"
#endif

/* Moves the pointer past leading spaces without modifying the string. */
static char *skip_left(char *string)
{
    while (isspace((unsigned char)*string)) {
        string++;
    }
    return string;
}

/* Removes trailing spaces in place so keys and values can be compared safely. */
static void trim_right(char *string)
{
    size_t length;

    length = strlen(string);
    while (length > 0 && isspace((unsigned char)string[length - 1])) {
        string[length - 1] = '\0';
        length--;
    }
}

/* Allocates a private copy because each coefficient expression is owned by problem. */
static char *copy_string(const char *string)
{
    size_t length;
    char *copy;

    length = strlen(string);
    copy = malloc(length + 1);
    if (copy) {
        memcpy(copy, string, length + 1);
    }
    return copy;
}

/* Replaces one owned expression string only after the new allocation succeeds. */
static bool replace_expression(char **destination, const char *source)
{
    char *copy;

    copy = copy_string(source);
    if (!copy) {
        return false;
    }
    free(*destination);
    *destination = copy;
    return true;
}

/* Parses an integer and rejects malformed input, overflow, and trailing text. */
static bool parse_int(const char *text, int *value)
{
    char *endptr;
    long parsed;

    errno = 0;
    parsed = strtol(text, &endptr, 10);
    while (isspace((unsigned char)*endptr)) {
        endptr++;
    }
    if (errno != 0 || *text == '\0' || *endptr != '\0' ||
            parsed < -2147483647L || parsed > 2147483647L) {
        return false;
    }
    *value = (int)parsed;
    return true;
}

/* Parses a finite floating-point setting; NaN and infinity are not accepted. */
static bool parse_double(const char *text, double *value)
{
    char *endptr;
    double parsed;

    errno = 0;
    parsed = strtod(text, &endptr);
    while (isspace((unsigned char)*endptr)) {
        endptr++;
    }
    if (errno != 0 || *text == '\0' || *endptr != '\0' || !isfinite(parsed)) {
        return false;
    }
    *value = parsed;
    return true;
}

/*
 * Validates the complete problem after every field has been read.
 * The basis-size limit protects the educational dense Newton implementation
 * from allocating an impractically large Jacobian matrix.
 */
static bool validate_problem(const problem_t *problem)
{
    bool valid;
    int basis_size;

    if (!problem->a_expression || !problem->b_expression ||
            !problem->c_expression || problem->x1 <= problem->x0 ||
            problem->wavelet_order < 1 || problem->wavelet_order > 20 ||
            problem->wavelet_level < 0 || problem->wavelet_level > 6 ||
            problem->tolerance <= 0.0 || problem->max_iterations < 1 ||
            problem->output_points < 2) {
        return false;
    }

    basis_size = problem->wavelet_order * (1 << problem->wavelet_level);
    if (basis_size > 128) {
        /* Dense Newton matrices are intentionally limited in this teaching code. */
        return false;
    }

    (void)evaluate_expression(problem->a_expression, problem->x0, &valid);
    if (!valid) {
        return false;
    }
    (void)evaluate_expression(problem->b_expression, problem->x0, &valid);
    if (!valid) {
        return false;
    }
    (void)evaluate_expression(problem->c_expression, problem->x0, &valid);
    return valid;
}

/* Initializes every field so destroy_problem is safe after a partial read. */
void init_problem(problem_t *problem)
{
    problem->a_expression = NULL;
    problem->b_expression = NULL;
    problem->c_expression = NULL;
    problem->x0 = 0.0;
    problem->x1 = 1.0;
    problem->y0 = 0.0;
    problem->wavelet_order = 4;
    problem->wavelet_level = 2;
    problem->tolerance = 1e-10;
    problem->max_iterations = 50;
    problem->output_points = 201;
}

/*
 * Reads one key=value configuration file.  Empty lines and lines beginning
 * with '#' are comments.  A spelling error in a key is treated as an error,
 * because ignoring it could silently change the numerical method.
 */
bool read_problem(const char *filename, problem_t *problem)
{
    FILE *file;
    char *line;
    size_t line_size;
    char *key;
    char *value;
    char *equals;
    bool success;

    file = fopen(filename, "r");
    if (!file) {
        return false;
    }

#ifdef DEBUG
    if (logfile) {
        fprintf(logfile, "The program started reading problem file: %s.\n",
                filename);
    }
#endif

    success = true;
    line = NULL;
    line_size = 0;

    while (getline(&line, &line_size, file) != -1) {
        key = skip_left(line);
        trim_right(key);

        if (*key == '\0' || *key == '#') {
            /* Empty lines and full-line comments do not change the problem. */
            continue;
        }

        /* Split the line into the setting name and its value. */
        equals = strchr(key, '=');
        if (!equals) {
            success = false;
            break;
        }
        *equals = '\0';
        value = skip_left(equals + 1);
        trim_right(key);
        trim_right(value);

        if (strcmp(key, "a") == 0) {
            success = replace_expression(&problem->a_expression, value);
        } else if (strcmp(key, "b") == 0) {
            success = replace_expression(&problem->b_expression, value);
        } else if (strcmp(key, "c") == 0) {
            success = replace_expression(&problem->c_expression, value);
        } else if (strcmp(key, "x0") == 0) {
            success = parse_double(value, &problem->x0);
        } else if (strcmp(key, "x1") == 0) {
            success = parse_double(value, &problem->x1);
        } else if (strcmp(key, "y0") == 0) {
            success = parse_double(value, &problem->y0);
        } else if (strcmp(key, "wavelet_order") == 0) {
            success = parse_int(value, &problem->wavelet_order);
        } else if (strcmp(key, "wavelet_level") == 0) {
            success = parse_int(value, &problem->wavelet_level);
        } else if (strcmp(key, "tolerance") == 0) {
            success = parse_double(value, &problem->tolerance);
        } else if (strcmp(key, "max_iterations") == 0) {
            success = parse_int(value, &problem->max_iterations);
        } else if (strcmp(key, "output_points") == 0) {
            success = parse_int(value, &problem->output_points);
        } else {
            /* Unknown fields are errors so a misspelled setting is not ignored. */
            success = false;
        }

        if (!success) {
            break;
        }
    }

    free(line);
    fclose(file);

    if (success) {
        success = validate_problem(problem);
    }

#ifdef DEBUG
    if (logfile) {
        fprintf(logfile, "Problem file reading %s.\n",
                success ? "succeeded" : "failed");
    }
#endif

    return success;
}

/* Frees the three expressions that are allocated while the file is read. */
void destroy_problem(problem_t *problem)
{
    free(problem->a_expression);
    free(problem->b_expression);
    free(problem->c_expression);
    problem->a_expression = NULL;
    problem->b_expression = NULL;
    problem->c_expression = NULL;
}
