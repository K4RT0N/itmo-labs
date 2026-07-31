#include <stdlib.h>
#include <stdio.h>

#include "boyer_moore.h"
#include "introsort.h"
#include "read_data.h"
#include "messages.h"
#include "args.h"

#ifdef DEBUG
#include "logfile.h"
#endif

void print_args(args_t *args);

int main(int argc, char **argv)
{
    args_t args;

    FILE *f;

    deque_t deque;

#ifdef DEBUG
    logfile = fopen("logfile", "w");
    if (logfile) {
        setvbuf(logfile, NULL, _IONBF, 0);
        fprintf(logfile, "The program has started.\n");
    } else {
        fprintf(stderr, "An error occured while opening the logfile.\n");
    }
#endif

    /* Parse arguments */
    init_args(argc, argv, &args);

    if (!args.valid_usage) {
        /* Invalid usage */
        usage_message();
#ifdef DEBUG
        if (logfile) {
            fprintf(logfile, "The program was stopped because command line "
                    "arguments are invalid.\n");
            fclose(logfile);
        }
#endif
        exit(1);
    }

    init_deque(&deque);

    /* Opening file for reading */
    f = fopen(args.filename, "r");
    if (!f) {
        /* An error occured while trying to open file */
#ifdef DEBUG
        if (logfile) {
            fprintf(logfile, "An error occured while trying to open file %s.\n",
                    args.filename);
            fclose(logfile);
        }
#endif
        fprintf(stderr, "An error occured while trying to open %s\n",
                args.filename);
        destroy_args(&args);
        exit(2);
    }

#ifdef DEBUG
    if (logfile) {
        fprintf(logfile, "The input file %s was opened successfully.\n",
                args.filename);
    }
#endif

    /* Reading numbers from file and saving them in deque */
    read_data(f, &deque);

    /* Introsort algorithm */
    introsort(&deque);

    /* Finding all dictionary entries in file */
    fseek(f, 0, SEEK_SET);
#ifdef DEBUG
    if (logfile) {
        fprintf(logfile, "The input file position was reset for substring search.\n");
    }
#endif
    find_all_entries(f, args.dictionary, args.dictionary_length);

    while (!is_empty(&deque)) {
        printf("%lf\n", pop_front(&deque));
    }

#ifdef DEBUG
    if (logfile) {
        fprintf(logfile, "Sorted deque content was printed.\n");
    }
#endif

    destroy_args(&args);
    fclose(f);

#ifdef DEBUG
    if (logfile) {
        fprintf(logfile, "The program completed successfully.\n");
        fclose(logfile);
    }
#endif

    return 0;
}

void print_args(args_t *args)
{
    if (args->valid_usage) {
        printf("Valid usage\n");
    } else {
        printf("Invalid usage\n");
    }

    if (args->filename) {
        printf("%s\n", args->filename);
    }

    for (int i = 0; i < args->dictionary_length; i++) {
        printf("%s\n", args->dictionary[i]);
    }
}
