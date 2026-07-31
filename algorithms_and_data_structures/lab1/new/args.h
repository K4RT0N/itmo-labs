#ifndef _ARGS_H
#define _ARGS_H

#include <stdbool.h>

#include "array.h"

typedef struct args_t {
    array_type_t array_type;
    bool valid_usage;
    char *filename;
} args_t;

/*
 * Parses command line arguments and saves results in args_t variable
 */
void parse_args(int argc, char **argv, args_t *args); // (44)

#endif