#include <stdlib.h>
#include <stdio.h>

#include "read_data.h"
#include "cqueue.h"

#ifdef DEBUG
#include "logfile.h"
#endif

/*
 * Reads the file one line at a time.  Only a complete numeric line is passed
 * to push; invalid lines remain available for the later substring search but
 * do not participate in sorting.
 */
void read_data(FILE *f, cqueue_t *cqueue)
{
    char *lineptr;
    size_t n;
    size_t line_number;
    long accepted_count;

    double number;
    char *endptr;

    lineptr = NULL;
    n = 0;
    line_number = 1;
    accepted_count = 0;

#ifdef DEBUG
    if (logfile) {
        fprintf(logfile, "File reading for numeric values started.\n");
    }
#endif

    /*
     * getline owns and enlarges lineptr as necessary.  Its -1 result means
     * end-of-file or a read error, either of which ends this pass.
     */
    while (getline(&lineptr, &n, f) != -1) {
        /*
         * strtod leaves endptr at the first character that was not part of
         * the number.  The original format accepts a value only when that
         * character is the line-feed terminating the input line.
         */
        number = strtod(lineptr, &endptr);
        if (lineptr == endptr || *endptr != '\n') {
#ifdef DEBUG
            if (logfile) {
                fprintf(logfile, "Line %zu is not a standalone number and "
                        "was skipped.\n", line_number);
            }
#endif
            line_number++;
            continue;
        }

        /* A valid number is retained subject to the queue capacity rule. */
        push(cqueue, number);
        accepted_count++;
        line_number++;
    }

    /* getline allocated lineptr, so this module releases it after the pass. */
    free(lineptr);

#ifdef DEBUG
    if (logfile) {
        fprintf(logfile, "Numeric file reading ended. Accepted values: %ld. "
                "Retained values: %ld.\n", accepted_count, cqueue->length);
    }
#endif
}
