#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#include "codes.h"
#include "read_data.h"

static double parse_number(char *line, bool *result);

/*
 * Reads data from file with name filename into static or dynamic array.
 */
code_t read_data(double **array, int *length, char *filename, bool dynamic)
{
    char *lineptr;
    double number;
    bool result;
    size_t n;
    FILE *f;
    int i;

#   ifdef DEBUG
    fprintf(stderr, "read_data function started.\n");
#   endif
    
    /*
     * Opening the file.
     */
    f = fopen(filename, "r");
    if (f == NULL) {
        /*
         * Couldn't open the file.
         */
#       ifdef DEBUG
        fprintf(stderr, "Couldn't open file %s.\nread_data function ended.\n",
                filename);
#       endif

        return open_file_error_code;
    }

#   ifdef DEBUG
    fprintf(stderr, "File %s was succesfully opened for reading.\n", filename);
#   endif
    
    lineptr = NULL;
    i = 0;

#   ifdef DEBUG
    fprintf(stderr, "The file reading started.\n");
#   endif

    while (getline(&lineptr, &n, f) != -1) {
        /*
         * Reading file line by line.
         */

        if (i == *length) {
            /*
             * No more space.
             */
            if (!dynamic) {
                /*
                 * Impossible to make static array bigger in size.
                 */
#               ifdef DEBUG
                fprintf(stderr, "No more space in static array.\n");
#               endif

                free(lineptr);

#               ifdef DEBUG
                fprintf(stderr, "read_data function ended.\n");
#               endif

                return no_space_error_code;
            }
#           ifdef DEBUG
            fprintf(stderr, "No more space in dynamic array.\n");
#           endif
            
            /*
             * Reallocating dynamic array to make more space.
             */
            *length *= 2;
            *array = realloc(*array, *length * sizeof(double));

#           ifdef DEBUG
            fprintf(stderr, "The array length is doubled. "
                            "The new length is %d.\n", *length);
#           endif
        }
        number = parse_number(lineptr, &result);
        if (!result) {
            /*
             * The line doesn't satisfy the format requirements.
             */
#           ifdef DEBUG
            fprintf(stderr, "Couldn't parse line from file. "
                            "Invalid file format.\n");
#           endif

            free(lineptr);

#           ifdef DEBUG
            fprintf(stderr, "read_data function ended.\n");
#           endif

            return file_format_error_code;
        }
        (*array)[i] = number;
        i++;

        free(lineptr);
        lineptr = NULL;
    }

#   ifdef DEBUG
    fprintf(stderr, "The file reading ended.\n");
#   endif

    if (*length != i && dynamic) {
        /*
         * Truncation the dynamic array to the length required to store
         * all elements.
         */
        *length = i;
        *array = realloc(*array, sizeof(double) * i);

#       ifdef DEBUG
        fprintf(stderr, "The array is truncated to the length "
                        "required to store all read elements.\n");
#       endif
    }

#   ifdef DEBUG
    fprintf(stderr, "The resulting amount of elements is %d.\n", i);
#   endif

#   ifdef DEBUG
    fprintf(stderr, "read_data function ended.\n");
#   endif

    return success_code;
}

/*
 * Parses line and returns its numeric value.
 */
static double parse_number(char *line, bool *result)
{
    double number;
    char *endptr;

    number = strtod(line, &endptr);

    if (*endptr != '\n' || endptr == line) {
        *result = false;
    } else {
        *result = true;
    }
    
    return number;
}
