#ifndef _ARGS_H
#define _ARGS_H

#include <stdbool.h>

/*
 * Parsed command-line configuration.
 *
 * filename and every dictionary item point directly into argv, so this
 * structure owns only the dynamically allocated dictionary pointer array.
 * destroy_args frees that array but does not free the strings themselves.
 */
typedef struct {
    bool valid_usage;       /* True only when all mandatory options are valid. */
    char *filename;         /* Name of the input file supplied with -f. */
    long cqueue_length;     /* Maximum number of values retained by cqueue. */
    char **dictionary;      /* Array of search patterns taken from argv. */
    int dictionary_length;  /* Number of items stored in dictionary. */
} args_t;

/* Parses command-line options and stores the result in args. */
void init_args(int argc, char **argv, args_t *args);

/* Releases memory allocated for the dictionary pointer array. */
void destroy_args(args_t *args);

#endif
