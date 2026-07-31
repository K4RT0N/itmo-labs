#include <getopt.h>
#include <stddef.h>

#ifdef DEBUG
#include <stdio.h>
#endif

#include "args.h"

/*
 * Parses command line arguments and modifies args_t variable accordingly.
 */
void parse_args(int argc, char **argv, args_t *args) // (31)
{
    bool correct_usage;
    bool file_flag;

    char *filename;

    int opt;

    enum {
        file_val, unknown_val
    };
    struct option long_options[] = {
        {"file", required_argument, 0, file_val},
        {NULL, no_argument, 0, unknown_val}
    };

#   ifdef DEBUG
    fprintf(stderr, "parse_args function started.\n");
#   endif

    /*
     * Variable initialization
     */
    correct_usage = true; // (1)
    file_flag = false; // (1)
    filename = NULL; // (1)
    
    while ((opt = getopt_long(argc, argv, "f:", long_options, NULL)) != -1) { // (3)
        switch (opt) {
            case file_val: // (1)
            case 'f': // (1)
                /*
                 * User specified file name.
                 */
#               ifdef DEBUG
                fprintf(stderr, "User specified filename. "
                                "The filename is %s\n", optarg);
#               endif

                if (file_flag) { // (1)
                    /*
                     * User specified file name more than once. Invalid usage.
                     */
#                   ifdef DEBUG
                    fprintf(stderr, "User specified filename "
                                    "more than once. Invalid usage.\n");
#                   endif
                    correct_usage = false; // (1)
                    break; // (1)
                }
                file_flag = true; // (1)
                filename = optarg; // (1)
                break; // (1)
            default:
#               ifdef DEBUG
                fprintf(stderr, "Invalid usage.\n");
#               endif
                correct_usage = false; // (1)
                break; // (1)
        }
    } // (6)

    if (optind != argc) { // (1)
        /*
         * Not every word in command line input was parsed. Invalid usage.
         */
#       ifdef DEBUG
        fprintf(stderr, "Not every argument is parsable. Invalid usage.\n");
#       endif
        correct_usage = false; // (1)
    }

    /*
     * Saving parsed information into args_t variable.
     */
    args->correct_usage = correct_usage; // (3)
    args->file_flag = file_flag; // (3)
    args->filename = filename; // (3)

#   ifdef DEBUG
    fprintf(stderr, "parse_args function ended.\n");
#   endif
}

/*
 * Tells calling function if usage is correct.
 */
bool is_correct(args_t *args) // (4)
{
    bool result;

#   ifdef DEBUG
    fprintf(stderr, "is_correct function started.\n");
#   endif

    result = args->correct_usage; // (3)

#   ifdef DEBUG
    fprintf(stderr, "is_correct function ended. ");
    if (result) {
        fprintf(stderr, "The result is posivive.\n");
    } else {
        fprintf(stderr, "The result is negative.\n");
    }
#   endif

    return result; // (1)
}

/*
 * Gives calling function a filename from args_t variable
 */
char *get_filename(args_t *args, char *default_name) // (7)
{
    char *filename;

#   ifdef DEBUG
    fprintf(stderr, "get_filename function started.\n");
#   endif

    if (args->filename) { // (3)
        filename = args->filename; // (3)
    } else {
        filename = default_name; // (1)
    }

#   ifdef DEBUG
    fprintf(stderr, "get_filename function ended. The result is %s.\n",
            filename);
#   endif

    return filename; // (1)
}
