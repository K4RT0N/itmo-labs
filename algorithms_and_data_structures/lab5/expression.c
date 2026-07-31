#include <ctype.h>
#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "expression.h"

/*
 * Parser state shared by all recursive-descent functions.
 * position always points to the first not-yet-consumed character, x is the
 * current independent variable, and valid becomes false after any syntax or
 * domain error detected during parsing.
 */
typedef struct {
    const char *position;
    double x;
    bool valid;
} parser_t;

/* Skips all whitespace characters at the current parser position. */
static void skip_spaces(parser_t *parser)
{
    while (isspace((unsigned char)*parser->position)) {
        parser->position++;
    }
}

/* Parses a complete expression with addition and subtraction. */
static double parse_sum(parser_t *parser);

/* Parses a product with multiplication and division. */
static double parse_product(parser_t *parser);

/* Parses a right-associative power expression. */
static double parse_power(parser_t *parser);

/* Parses unary plus and unary minus. */
static double parse_unary(parser_t *parser);

/* Parses numbers, x, constants, functions and parenthesized expressions. */
static double parse_primary(parser_t *parser);

/*
 * Applies one supported function after its argument has already been parsed.
 * Unknown identifiers are rejected here instead of being silently interpreted
 * as zero; this makes misspelled coefficients visible to the user.
 */
static double apply_function(const char *name, double value, bool *valid)
{
    if (strcmp(name, "sin") == 0) {
        return sin(value);
    }
    if (strcmp(name, "cos") == 0) {
        return cos(value);
    }
    if (strcmp(name, "tan") == 0) {
        return tan(value);
    }
    if (strcmp(name, "exp") == 0) {
        return exp(value);
    }
    if (strcmp(name, "log") == 0 || strcmp(name, "ln") == 0) {
        return log(value);
    }
    if (strcmp(name, "sqrt") == 0) {
        return sqrt(value);
    }
    if (strcmp(name, "abs") == 0) {
        return fabs(value);
    }

    *valid = false;
    return 0.0;
}

/*
 * Reads one identifier consisting of letters, digits and underscores.
 * Characters are converted to lower case so that function names are handled
 * consistently, independently of their spelling in the input file.
 */
static void parse_identifier(parser_t *parser, char *buffer, size_t size)
{
    size_t length;

    length = 0;
    while (isalnum((unsigned char)*parser->position) ||
            *parser->position == '_') {
        if (length + 1 < size) {
            buffer[length] = (char)tolower((unsigned char)*parser->position);
            length++;
        } else {
            parser->valid = false;
        }
        parser->position++;
    }
    buffer[length] = '\0';
}

/* Implements the lowest-precedence level: term { (+|-) term }. */
static double parse_sum(parser_t *parser)
{
    double value;

    value = parse_product(parser);
    while (parser->valid) {
        skip_spaces(parser);
        if (*parser->position == '+') {
            parser->position++;
            value += parse_product(parser);
        } else if (*parser->position == '-') {
            parser->position++;
            value -= parse_product(parser);
        } else {
            break;
        }
    }

    return value;
}

/* Implements multiplication and division after powers are formed. */
static double parse_product(parser_t *parser)
{
    double value;
    double divisor;

    value = parse_power(parser);
    while (parser->valid) {
        skip_spaces(parser);
        if (*parser->position == '*') {
            parser->position++;
            value *= parse_power(parser);
        } else if (*parser->position == '/') {
            parser->position++;
            divisor = parse_power(parser);
            if (divisor == 0.0) {
                parser->valid = false;
                return 0.0;
            }
            value /= divisor;
        } else {
            break;
        }
    }

    return value;
}

/* Uses recursive parsing on the right to make a^b^c equal a^(b^c). */
static double parse_power(parser_t *parser)
{
    double base;
    double exponent;

    base = parse_unary(parser);
    skip_spaces(parser);
    if (parser->valid && *parser->position == '^') {
        parser->position++;
        exponent = parse_power(parser);
        base = pow(base, exponent);
    }

    return base;
}

/* Handles one or more unary signs before a primary expression. */
static double parse_unary(parser_t *parser)
{
    skip_spaces(parser);
    if (*parser->position == '+') {
        parser->position++;
        return parse_unary(parser);
    }
    if (*parser->position == '-') {
        parser->position++;
        return -parse_unary(parser);
    }

    return parse_primary(parser);
}

/*
 * Parses the elementary building blocks: parentheses, numbers, constants,
 * the variable x, and calls of one-argument mathematical functions.
 */
static double parse_primary(parser_t *parser)
{
    char *endptr;
    char identifier[32];
    double value;

    skip_spaces(parser);

    if (*parser->position == '(') {
        parser->position++;
        value = parse_sum(parser);
        skip_spaces(parser);
        if (*parser->position != ')') {
            parser->valid = false;
            return 0.0;
        }
        parser->position++;
        return value;
    }

    if (isdigit((unsigned char)*parser->position) || *parser->position == '.') {
        value = strtod(parser->position, &endptr);
        if (endptr == parser->position) {
            parser->valid = false;
            return 0.0;
        }
        parser->position = endptr;
        return value;
    }

    if (isalpha((unsigned char)*parser->position)) {
        parse_identifier(parser, identifier, sizeof(identifier));
        if (strcmp(identifier, "x") == 0) {
            return parser->x;
        }
        if (strcmp(identifier, "pi") == 0) {
            return acos(-1.0);
        }
        if (strcmp(identifier, "e") == 0) {
            return exp(1.0);
        }

        skip_spaces(parser);
        if (*parser->position != '(') {
            parser->valid = false;
            return 0.0;
        }
        parser->position++;
        value = parse_sum(parser);
        skip_spaces(parser);
        if (*parser->position != ')') {
            parser->valid = false;
            return 0.0;
        }
        parser->position++;
        return apply_function(identifier, value, &parser->valid);
    }

    parser->valid = false;
    return 0.0;
}

/*
 * Evaluates the complete expression and verifies that no unconsumed text
 * remains.  A non-finite result is also rejected because it would invalidate
 * the Riccati residual and the Newton matrix.
 */
double evaluate_expression(const char *expression, double x, bool *valid)
{
    parser_t parser;
    double value;

    parser.position = expression;
    parser.x = x;
    parser.valid = true;

    value = parse_sum(&parser);
    skip_spaces(&parser);
    if (*parser.position != '\0' || !isfinite(value)) {
        parser.valid = false;
    }

    if (valid) {
        *valid = parser.valid;
    }

    return parser.valid ? value : 0.0;
}
