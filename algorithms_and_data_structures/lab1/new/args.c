#include <stdbool.h>
#include <getopt.h>
#include <stddef.h>

#include "logfile.h"
#include "args.h"

/*
 * Parses command line arguments and saves results in args_t variable
 */
void parse_args(int argc, char **argv, args_t *args) // (52)
{
    /*
     * Storing getopt_long return values
     */
    int opt;

    /*
     * Storing the results of parsing command line arguments
     */
    bool valid_usage;
    bool dynamic_flag;
    bool static_flag;
    char *filename;

    /*
     * Values returned by getopt_long function
     */
    enum {
        static_val, dynamic_val, filename_val, undefined_val
    };

    /*
     * Long options passed to getopt_long
     */
    struct option longopts[] = {
        {"static", no_argument, NULL, static_val},
        {"dynamic", no_argument, NULL, dynamic_val},
        {"filename", required_argument, NULL, filename_val},
        {NULL, no_argument, NULL, undefined_val}
    };

#   ifdef DEBUG
    if (logfile) {
        fprintf(logfile, "The program started parsing arguments.\n");
    }
#   endif

    /*
     * Disable getopt_long error messages
     */
    opterr = 0; // (1)

    /*
     * Setting initial values ​​of variables
     */
    valid_usage = true; // (1)
    dynamic_flag = false; // (1)
    static_flag = false; // (1)
    filename = NULL; // (1)

    while ((opt = getopt_long(argc, argv, "f:sd", longopts, NULL)) != -1) { // (3) * 3 = (9)
        if (opt == static_val || opt == 's') { // (3)
            /*
             * The user used one of the static options
             */
#           ifdef DEBUG
            if (logfile) {
                fprintf(logfile, "The user selected a "
                                 "static array type.\n");
            }
#           endif
            if (static_flag) { // (1)
                /*
                 * The user used static optins more than once
                 * Invalid usage
                 */
#               ifdef DEBUG
                if (logfile) {
                    fprintf(logfile, "The user selected a static "
                                     "array more than one. Invalid "
                                     "usage detected\n");
                }
#               endif
                valid_usage = false; // (1)
            } else {
                static_flag = true; // (1)
            }
        } else if (opt == dynamic_val || opt == 'd') { // (3)
            /*
             * The user used one of the dynamic options
             */
#           ifdef DEBUG
            if (logfile) {
                fprintf(logfile, "The user selected a "
                                 "dynamic array type.\n");
            }
#           endif
            if (dynamic_flag) { // (1)
                /*
                 * The user used dynamic options more than once
                 * Invalid usage
                 */
#               ifdef DEBUG
                if (logfile) {
                    fprintf(logfile, "The user selected a dynamic "
                                     "array more than once. Invalid "
                                     "usage detected\n");
                }
#               endif
                valid_usage = false; // (1)
            } else {
                dynamic_flag = true; // (1)
            }
        } else if (opt == filename_val || opt == 'f') { // (3)
            /*
             * The user used on of the file name options
             */
#           ifdef DEBUG
            if (logfile) {
                fprintf(logfile, "The user specified a file name: %s.\n",
                        optarg);
            }
#           endif
            if (filename) { // (1)
                /*
                 * The user used file name options more than once
                 * Invalid usage
                 */
#               ifdef DEBUG
                if (logfile) {
                    fprintf(logfile, "The user specified a "
                                     "file more than once. "
                                     "Invalid usage detected.\n");
                }
#               endif
                valid_usage = false; // (1)
            } else {
                filename = optarg; // (1)
            }
        } else {
            /*
             * The user passed undefined option
             * Invalid usage
             */
#           ifdef DEBUG
            if (logfile) {
                fprintf(logfile, "The user submitted an undefined option. "
                                 "Invalid usage detected.\n");
            }
#           endif
            valid_usage = false; // (1)
        }
    } // (22)

    if ((dynamic_flag && static_flag) || !filename) { // (3)
        /*
         * The user selected both array types or didn't specify file name
         * Invalid usage
         */
#       ifdef DEBUG
        if (logfile) {
            if (dynamic_flag && static_flag) {
                fprintf(logfile, "The user selected both dynamic and static "
                                 "array types. Invalid usage detected.\n");
            }
            if (!filename) {
                fprintf(logfile, "The user didn't specify file name. "
                                 "Invalid usage detected.\n");
            }
        }
#       endif
        valid_usage = false; // (1)
    } // (1)

    /*
     * Saving command line arguments parsing in args_t variable
     */
    if (!dynamic_flag) { // (2)
        args->array_type = static_array_type; // (3)
    } else if(!static_flag) { // (2)
        args->array_type = dynamic_array_type; // (3)
    } else {
        args->array_type = undefined_array_type; // (3)
    }
    args->valid_usage = valid_usage; // (3)
    args->filename = filename; // (3)

#   ifdef DEBUG
    if (logfile) {
        switch (args->array_type) {
            case static_array_type:
                fprintf(logfile, "Array type: static\n");
                break;
            case dynamic_array_type:
                fprintf(logfile, "Array type: dynamic\n");
                break;
            case undefined_array_type:
                fprintf(logfile, "Array type: undefined\n");
                break;
        }
        fprintf(logfile, "Usage: %s\n", valid_usage ? "valid" : "invalid");
        fprintf(logfile, "File name: %s", filename ? filename : "undefined");
    }
#   endif
}