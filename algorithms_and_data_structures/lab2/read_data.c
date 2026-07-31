#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#include "codes.h"
#include "read_data.h"

static double parse_number(char *line, bool *result);

/*
 * Reads data from file with name filename into static or dynamic array.
 */
code_t read_data(stack_t *stack, char *filename) // 4n + 30
{
    char *lineptr;
    double number;
    bool result;
    size_t n;
    FILE *f;
#   ifdef DEBUG
    int i;
#   endif

#   ifdef DEBUG
    fprintf(stderr, "read_data function started.\n");
#   endif
    
    /*
     * Opening the file.
     */
    f = fopen(filename, "r"); // (2)
    if (f == NULL) { // (1)
        /*
         * Couldn't open the file.
         */
#       ifdef DEBUG
        fprintf(stderr, "Couldn't open file %s.\nread_data function ended.\n",
                filename);
#       endif

        return open_file_error_code; // (1)
    }

#   ifdef DEBUG
    fprintf(stderr, "File %s was succesfully opened for reading.\n", filename);
#   endif
    
    lineptr = NULL; // (1)
#   ifdef DEBUG
    i = 0;
#   endif

#   ifdef DEBUG
    fprintf(stderr, "The file reading started.\n");
#   endif

    while (getline(&lineptr, &n, f) != -1) { // 4n
        /*
         * Reading file line by line.
         */
        number = parse_number(lineptr, &result); // (3)
        if (!result) { // (1)
            /*
             * The line doesn't satisfy the format requirements.
             */
#           ifdef DEBUG
            fprintf(stderr, "Couldn't parse line from file. "
                            "Invalid file format.\n");
#           endif

            free(lineptr); // (1)

#           ifdef DEBUG
            fprintf(stderr, "read_data function ended.\n");
#           endif

            return file_format_error_code; // (1)
        }
        push(stack, number); // (19)
#       ifdef DEBUG
        i = i + 1;
#       endif

        free(lineptr); // (1)
        lineptr = NULL; // (1)
    } // 25

#   ifdef DEBUG
    fprintf(stderr, "The file reading ended.\n");
#   endif

#   ifdef DEBUG
    fprintf(stderr, "The resulting amount of elements is %d.\n", i);
#   endif

#   ifdef DEBUG
    fprintf(stderr, "read_data function ended.\n");
#   endif

    return success_code; // (1)
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
