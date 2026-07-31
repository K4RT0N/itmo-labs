#include <stdlib.h>
#include <stdio.h>

#include "read_data.h"
#include "deque.h"

#ifdef DEBUG
#include "logfile.h"
#endif

/* Reads numbers from files and pushes them into deque */
void read_data(FILE *f, deque_t *deque)
{
    char *lineptr;;
    size_t n;

    double number;
    char *endptr;
    long number_count;
    long invalid_line_count;

#ifdef DEBUG
    if (logfile) {
        fprintf(logfile, "Program started reading file line-by-line.\n");
    }
#endif

    number_count = 0;
    invalid_line_count = 0;
    lineptr = NULL;
    /* Gegging a line from file one by one */
    while (getline(&lineptr, &n, f) != -1) {
        /* Trying to parse line as a number */
        number = strtod(lineptr, &endptr);
        if (lineptr == endptr || *endptr != '\n') {
            /* Parsing failed
             * Skipping line */
            invalid_line_count++;
#ifdef DEBUG
            if (logfile) {
                fprintf(logfile, "The file contains an invalid number. "
                        "The line was skipped.\n");
            }
#endif
            continue;
        }
        /* Pushing number into deque */
        push_back(deque, number);
        number_count++;
    }
    /* Freeing allocated memory */
    free(lineptr);

#ifdef DEBUG
    if (logfile) {
        fprintf(logfile, "The resulting number count: %ld.\n", number_count);
        fprintf(logfile, "Skipped invalid line count: %ld.\n", invalid_line_count);
        fprintf(logfile, "File reading succeeded.\n");
    }
#endif
}
