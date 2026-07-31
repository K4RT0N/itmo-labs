#include <getopt.h>
#include <stdlib.h>

#ifdef DEBUG
#include <stdio.h>
#endif

#include "args.h"

/*
 * Parses command line arguments and modifies args_t variable accordingly.
 */
void parse_args(int argc, char **argv, args_t *args)
{
    bool correct_usage;
    bool dynamic_flag;
    bool static_flag;
    bool file_flag;

    char *filename;
    
    int opt;

    enum {
        static_val, dynamic_val, file_val, unknown_val
    };
    struct option long_options[] = {
        {"static", no_argument, 0, static_val},
        {"dynamic", no_argument, 0, dynamic_val},
        {"file", required_argument, 0, file_val},
        {NULL, no_argument, 0, unknown_val}
    };

#   ifdef DEBUG
    fprintf(stderr, "parse_args function started.\n");
#   endif
   
    /*
     * Variable initialization.
     */
    correct_usage = true;
    dynamic_flag = false;
    static_flag = false;
    file_flag = false;
    filename = NULL;

    while ((opt = getopt_long(argc, argv, "sdf:", long_options, NULL)) != -1) {
        switch (opt) {
            case static_val:
            case 's':
                /*
                 * User specified static array.
                 */
#               ifdef DEBUG
                fprintf(stderr, "User specified static array.\n");
#               endif
                static_flag = true;
                break;
            case dynamic_val:
            case 'd':
                /*
                 * User specified dynamic array.
                 */
#               ifdef DEBUG
                fprintf(stderr, "User specified dynamic array.\n");
#               endif
                dynamic_flag = true;
                break;
            case file_val:
            case 'f':
                /*
                 * User specified file name.
                 */
#               ifdef DEBUG
                fprintf(stderr, "User specified filename. "
                                "The filename is %s\n", optarg);
#               endif
                if (file_flag) {
                    /*
                     * User specified file name more than once. Invalid usage.
                     */
#                   ifdef DEBUG
                    fprintf(stderr, "User specified filename "
                                    "more than once. Invalid usage.\n");
#                   endif
                    correct_usage = false;
                    break;
                }
                file_flag = true;
                filename = optarg;
                break;
            default:
#               ifdef DEBUG
                fprintf(stderr, "Invalid usage.\n");
#               endif
                correct_usage = false;
                break;
        }
    }
    
    if (static_flag && dynamic_flag) {
        /*
         * User specified array to be both static and dynamic. Invalid usage
         */
#       ifdef DEBUG
        fprintf(stderr, "User specified both static and dynamic array at "
                        "the same time. Invalid usage.\n");
#       endif
        correct_usage = false;
    }
    if (optind != argc) {
        /*
         * Not every word in command line input was parsed. Invalid usage.
         */
#       ifdef DEBUG
        fprintf(stderr, "Not every argument is parsable. Invalid usage.\n");
#       endif
        correct_usage = false;
    }

    /*
     * Saving parsed information into args_t variable.
     */
    args->correct_usage = correct_usage;
    args->dynamic_flag = dynamic_flag;
    args->static_flag = static_flag;
    args->file_flag = file_flag;
    args->filename = filename;

#   ifdef DEBUG
    fprintf(stderr, "parse_args function ended.\n");
#   endif
}

/*
 * Tells callign function if array is supposed to be dynamic.
 */
bool is_dynamic(args_t *args)
{
    bool result;

#   ifdef DEBUG
    fprintf(stderr, "is_dynamic function started.\n");
#   endif

    result = !args->static_flag;

#   ifdef DEBUG
    fprintf(stderr, "is_dynamic function ended. ");
    if (result) {
        fprintf(stderr, "The result is posivive.\n");
    } else {
        fprintf(stderr, "The result is negative.\n");
    }
#   endif

    return result;
}

/*
 * Tells callign function if array is supposed to be static.
 */
bool is_static(args_t *args)
{
    bool result;

#   ifdef DEBUG
    fprintf(stderr, "is_static function started.\n");
#   endif

    result = args->static_flag;

#   ifdef DEBUG
    fprintf(stderr, "is_static function ended. ");
    if (result) {
        fprintf(stderr, "The result is posivive.\n");
    } else {
        fprintf(stderr, "The result is negative.\n");
    }
#   endif

    return result;
}

/*
 * Tells calling function if usage is correct.
 */
bool is_correct(args_t *args)
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

    return result;
}

/*
 * Gives calling function a filename from args_t variable
 */
char *get_filename(args_t *args, char *default_name)
{
    char *filename;

#   ifdef DEBUG
    fprintf(stderr, "get_filename function started.\n");
#   endif

    filename = args->file_flag ? args->filename : default_name;

#   ifdef DEBUG
    fprintf(stderr, "get_filename function ended. The result is %s.\n",
            filename);
#   endif

    return filename;
}
