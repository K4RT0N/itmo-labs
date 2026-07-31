#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#include "read_data.h"
#include "array.h"

#ifdef DEBUG
#include "logfile.h"
#endif

/*
 * Reads data from the specified file and stores it in an array
 */
bool read_data(FILE *f, array_type_t array_type, double **array_ptr, int *length) // 21 + 36n
{
    /*
     * Variable associated with inserting to an array
     */
    double *array;
    double number;
    int i;

    /*
     * Variables associated with reading from the file
     */
    char *lineptr;
    char *endptr;
    size_t n;

#   ifdef DEBUG
    if (logfile) {
        fprintf(logfile, "Program started reading file line-by-line\n");
    }
#   endif

    if (array_type == undefined_array_type || *length < 1 || !array_ptr) { // (6)
        /*
         * Invalid array data was received
         */
#       ifdef DEBUG
        if (logfile) {
            fprintf(logfile, "Invalid array data was received\n"
                             "File reading failed.\n");
        }
#       endif
        return false; // (1)
    }

    /*
     * Settings variables ready for the first line
     */
    i = 0; // (1)
    lineptr = NULL; // (1)
    array = *array_ptr; // (2)
    /*
     * Reading cycle
     */
    while (getline(&lineptr, &n, f) != -1) { // (4n)
        number = strtod(lineptr, &endptr); // (2)
        if (*lineptr == *endptr || (*endptr != '\n' && *endptr != '\0')) { // (9)
            /*
             * The string turned out to be not a pure number
             * Invalid file format
             */
#           ifdef DEBUG
            if (logfile) {
                fprintf(logfile, "The file turned out to be of invalid format. "
                                 "Problematic line: %s\nFile reading failed.\n",
                        lineptr);
            }
#           endif
            free(lineptr); // (1)
            return false; // (1)
        }
        if (i == *length) { // (2)
            switch (array_type) {
                case static_array_type: // (1)
                    /*
                     * There is no more space in the array to store numbers
                     * The file is too big for a static array to handle
                     */
#                   ifdef DEBUG
                    if (logfile) {
                        fprintf(logfile, "There is no more space in the array "
                                         "to store numbers. The file is too "
                                         "big for a static array to hanble\n"
                                         "File reading failed.\n");
                    }
#                   endif
                    free(lineptr); // (1)
                    return false; // (1)
                    break;
                case dynamic_array_type: // (1)
                    /*
                     * There is no more space in the array to store number
                     * Dynamic array reallocation
                     */
#                   ifdef DEBUG
                    if (logfile) {
                        fprintf(logfile, "There is no more space in the array "
                                         "to store numbers.\nDynamic array "
                                         "reallocation...\n");
                    }
#                   endif
                    *length *= 2; // (4)
                    array = realloc(array, *length * sizeof(double)); // (4)
                    if (!array) { // (1)
#                       ifdef DEBUG
                        if (logfile) {
                            fprintf(logfile, "Dynamic array reallocation "
                                             "failed\nFile reading failed.\n");
                        }
#                       endif
                        free(lineptr); // (1)
                        return false; // (1)
                    }
#                   ifdef DEBUG
                    if (logfile) {
                        fprintf(logfile, "Dynamic array reallocation succeded. "
                                         "New length: %d.\n", *length);
                    }
#                   endif
                    *array_ptr = array; // (2)
                    break; // (1)
                default:
#                   ifdef DEBUG
                    if (logfile) {
                        fprintf(logfile, "Invalid array type detected.\n"
                                         "File reading failed.\n");
                    }
#                   endif
                    /*
                     * Invalid array type
                     */
                    free(lineptr); // (1)
                    break; // (1)
            }
        }

        /*
         * Inserting number into an array
         */
        array[i] = number; // (2)
        i++; // (2)

        /*
         * Setting variables ready for the next line
         */
        free(lineptr); // (1)
        lineptr = NULL; // (1)
    } // 32n

    /*
     * Storing the resulting length in the length variable
     */
#   ifdef DEBUG
    if (logfile) {
        fprintf(logfile, "The resulting length: %d.\n", i);
    }
#   endif
    *length = i; // (2) 
    if (array_type == dynamic_array_type) { // (1)
        /*
         * Truncating the dynamic array to the resulting length
         */
#       ifdef DEBUG
        if (logfile) {
            fprintf(logfile, "Dynamic array truncation...\n");
        }
#       endif
        array = realloc(array, *length * sizeof(double)); // (4)
        if (!array) { // (1)
#           ifdef DEBUG
            if (logfile) {
                fprintf(logfile, "Dynamic array truncation failed.\n"
                                 "File reading failed.\n");
            }
#           endif
            return false; // (1)
        }
#       ifdef DEBUG
        if (logfile) {
            fprintf(logfile, "Dynamic array truncation succeded.\n");
        }
#       endif
        *array_ptr = array; // (2)
    }

#   ifdef DEBUG
    if (logfile) {
        fprintf(logfile, "File reading succeded.\n");
    }
#   endif

    return true; // (1)
}