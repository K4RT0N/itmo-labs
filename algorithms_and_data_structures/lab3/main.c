#include <stdlib.h>
#include <stdio.h>

#include "aho_corasick.h"
#include "introsort.h"
#include "read_data.h"
#include "messages.h"
#include "args.h"

#ifdef DEBUG
#include "logfile.h"
#endif

#ifdef DEBUG
/*
 * Closes the shared log stream on every normal and error exit path.  The log
 * is optional: failure to create it must not prevent the main program from
 * carrying out its original task.
 */
static void close_logfile(void)
{
    int result;

    if (logfile) {
        result = fclose(logfile);
        if (result != 0) {
            fprintf(stderr, "An error occured while trying to close "
                    "the logfile.\n");
        }
        logfile = NULL;
    }
}
#endif

/*
 * Coordinates command-line parsing, numeric sorting and dictionary search.
 * Standard output preserves the original program format: pattern messages
 * appear first, followed by sorted values retained by the circular queue.
 */
int main(int argc, char **argv)
{
    args_t args;
    FILE *f;
    cqueue_t cqueue;

#ifdef DEBUG
    /*
     * The reference projects use a global, unbuffered "logfile" created only
     * for DEBUG builds.  It never replaces or redirects standard output.
     */
    logfile = fopen("logfile", "w");
    if (logfile) {
        setvbuf(logfile, NULL, _IONBF, 0);
        fprintf(logfile, "The program has started.\n");
    } else {
        fprintf(stderr, "An error occured while opening the logfile.\n");
    }
#endif

    /* Parse every command-line option before allocating queue resources. */
    init_args(argc, argv, &args);

    if (!args.valid_usage) {
        /*
         * Parsing owns a possible dictionary pointer array even on invalid
         * input, so release it before returning the original error code.
         */
        usage_message();
#ifdef DEBUG
        if (logfile) {
            fprintf(logfile, "Invalid usage detected. Program terminated.\n");
        }
#endif
        destroy_args(&args);
#ifdef DEBUG
        close_logfile();
#endif
        return 1;
    }

    /*
     * The queue capacity comes from the mandatory -n option.  Nodes are
     * allocated later by read_data as values are accepted from the file.
     */
    init_cqueue(&cqueue, args.cqueue_length);

    /* Open once for numbers, then rewind the same stream for text searching. */
    f = fopen(args.filename, "r");
    if (!f) {
        fprintf(stderr, "An error occured while trying to open %s\n",
                args.filename);
#ifdef DEBUG
        if (logfile) {
            fprintf(logfile, "An error occured while trying to open %s. "
                    "Program terminated.\n", args.filename);
        }
#endif
        destroy_cqueue(&cqueue);
        destroy_args(&args);
#ifdef DEBUG
        close_logfile();
#endif
        return 2;
    }

#ifdef DEBUG
    if (logfile) {
        fprintf(logfile, "File %s was successfully opened for reading.\n",
                args.filename);
    }
#endif

    /* First pass: retain valid numeric lines and sort the retained values. */
    read_data(f, &cqueue);
    introsort(&cqueue);

    /*
     * Second pass: search every line for dictionary entries.  fseek restores
     * the stream position after read_data reached the end of the input file.
     */
    if (fseek(f, 0, SEEK_SET) != 0) {
#ifdef DEBUG
        if (logfile) {
            fprintf(logfile, "Unable to rewind the input file before "
                    "dictionary search.\n");
        }
#endif
    } else {
        find_all_entries(f, args.dictionary, args.dictionary_length);
    }

    /*
     * pop returns values in the ascending order established by introsort.
     */
    while (!is_empty(&cqueue)) {
        printf("%lf\n", pop(&cqueue));
    }

    destroy_cqueue(&cqueue);
    destroy_args(&args);
    fclose(f);

#ifdef DEBUG
    if (logfile) {
        fprintf(logfile, "The program completed successfully.\n");
    }
    close_logfile();
#endif

    return 0;
}
