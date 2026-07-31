#ifndef PARSE_ARGS_H
#define PARSE_ARGS_H

#include <stdbool.h>

/*
 * Structure storing information about command line arguments.
 */
typedef struct args_t {
    /*
     * Is true, when program usage was correct. Is false otherwise.
     */
    bool correct_usage;

    /*
     * Flags indicating the usage of different keywords in command line.
     */
    bool file_flag;

    /*
     * Used for storing the filename value, if there is one in command line.
     * Is NULL otherwise.
     */
    char *filename;
} args_t;

/*
 * Parses command line arguments and modifies args_t variable accordingly.
 */
void parse_args(int argc, char **argv, args_t *args);

/*
 * Tells calling function if usage if correct.
 */
bool is_correct(args_t *args);

/*
 * Gives calling function a filename from args_t variable
 */
char *get_filename(args_t *args, char *default_name);

#endif
