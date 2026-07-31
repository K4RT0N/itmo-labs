#ifndef _ARGS_H
#define _ARGS_H

#include <stdbool.h>

/*
 * Command-line arguments needed by the solver.
 * filename is mandatory; output_filename receives a default when -o is absent.
 */
typedef struct {
    bool valid_usage;
    char *filename;
    char *output_filename;
} args_t;

/* Parses command-line arguments and saves them into args. */
void init_args(int argc, char **argv, args_t *args);

#endif
