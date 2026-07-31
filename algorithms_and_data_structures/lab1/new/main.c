#include <stdlib.h>
#include <stdio.h>

#include "introsort.h"
#include "read_data.h"
#include "errors.h"
#include "args.h"

#ifdef DEBUG
#include "logfile.h"
#endif

#ifndef STATIC_ARRAY_SIZE
#define STATIC_ARRAY_SIZE 1024
#endif

#ifndef INITIAL_DYNAMIC_ARRAY_SIZE
#define INITIAL_DYNAMIC_ARRAY_SIZE 64
#endif

int main(int argc, char **argv) // 560*n^2*log(n) - 142*n*log(n) + 611n - 8
{
    /*
     * Variables associated with command line arguments
     */
    args_t args;

    /*
     * Variables associated with file reding
     */
    int result;
    FILE *f;

    /*
     * Variables associated with arrays
     */
    double static_array[STATIC_ARRAY_SIZE];
    array_type_t array_type;
    double *array;
    int length;

#   ifdef DEBUG
    logfile = fopen("logfile", "w");
    if (logfile) {
        setvbuf(logfile, NULL, _IONBF, 0);
        fprintf(logfile, "The program has started.\n");
    } else {
        fprintf(stderr, "An error occured while opening the logfile.\n");
    }
#   endif

    /*
     * Parsing command line arguments
     */
    parse_args(argc, argv, &args); // 45

    if (!args.valid_usage) { // (2)
        /*
         * The user used program incorrectly
         */
        usage_error_message(); // (2)
        exit(usage_error_code); // (1)
    }

    f = fopen(args.filename, "r"); // (3)
    if (!f) { // (1)
        /*
         * An error occured while trying to open file
         */
#       ifdef DEBUG
        if (logfile) {
            fprintf(logfile, "An error occured while trying to "
                             "open file %s.\n", args.filename);
            result = fclose(logfile);
            if (result) {
                fprintf(stderr, "An error occured while trying to "
                                "close logfile.\n");
            }
        }
#       endif
        perror(args.filename); // (2)
        exit(file_open_error_code); // (1)
    }

    /*
     * Setting up an array
     */
    array_type = args.array_type; // (2)
    switch (array_type) {
        case static_array_type: // (1)
            /*
             * Setting up a static array
             */
            length = STATIC_ARRAY_SIZE; // (1)
            array = static_array; // (1)
#           ifdef DEBUG
            if (logfile) {
                fprintf(logfile, "The static array has been set up\n");
            }
#           endif
            break; // (1)
        case dynamic_array_type: // (1)
            /*
             * Setting up a dynamic array
             */
            length = INITIAL_DYNAMIC_ARRAY_SIZE; // (1)
            array = malloc(sizeof(double) * length); // (3)
            if (!array) { // (1)
                perror("Dynamic array"); // (1)
#               ifdef DEBUG
                if (logfile) {
                    fprintf(logfile, "An error occured while trying to "
                                     "allocate memory for dynamic array.\n");
                    result = fclose(logfile);
                    if (result) {
                        fprintf(stderr, "An error occured while "
                                        "trying to close logfile.\n");
                    }
                }
#               endif
                exit(memory_allocation_error_code); // (1)
            }
#           ifdef DEBUG
            if (logfile) {
                fprintf(logfile, "The dynamic array has been set up\n");
            }
#           endif
            break; // (1)
        case undefined_array_type: // (1)
            /*
             * Setting up an undefined array
             * Will most likely lead to an error
             */
#           ifdef DEBUG
            if (logfile) {
                fprintf(logfile, "The undefined array has been set up. "
                                 "Most likely will lead to an error\n");
            }
#           endif
            array = NULL; // (1)
            length = -1; // (1)
            break; // (1)
    }

    result = read_data(f, array_type, &array, &length); // (4)
    if (!result) { // (1)
        fprintf(stderr, "An error occured while trying to read numbers from "
                        "the file.\n"); // (1)
#       ifdef DEBUG
        if (logfile) {
            fprintf(logfile, "An error occured while trying to read file.\n");
            result = fclose(logfile);
            if (result) {
                fprintf(stderr, "An error occured while trying "
                                "to close logfile.\n");
            }
        }
#       endif
        exit(file_reading_error_code); // (1)
    }

    result = fclose(f); // (2)
    if (result) { // (1)
        /*
         * An error occured while trying to close file
         */
#       ifdef DEBUG
        if (logfile) {
            fprintf(logfile, "An error occured while trying to "
                             "close file %s.\n", args.filename);
            result = fclose(logfile);
            if (result) {
                fprintf(stderr, "An error occured while trying "
                                "to close logfile.\n");
            }
        }
#       endif
        perror(args.filename); // (2)
        exit(file_closure_error_code); // (1)
    }

    /*
     * Sorting numbers
     */
    introsort(array, length); // 560*n^2*log(n) - 142*n*log(n) + 606n - 74
    for (int i = 0; i < length; i++) { // 1 + 1 * n + 2(n - 1)
        printf("%lf\n", array[i]); // (2)
    } // 2n

#   ifdef DEBUG
    if (logfile) {
        fprintf(logfile, "The program completed successfully.\n");
        result = fclose(logfile);
        if (result) {
            fprintf(stderr, "An error occured while trying "
                            "to close logfile.\n");
        }
    }
#   endif
    
    return 0; // (1)
}
