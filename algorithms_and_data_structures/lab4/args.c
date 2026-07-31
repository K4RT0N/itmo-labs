#include <stdbool.h>
#include <getopt.h>
#include <stddef.h>
#include <stdlib.h>

#include "args.h"

#ifdef DEBUG
#include "logfile.h"
#endif

/* This function is used for extracting dictionary entities from
 * command line arguments. */
static void parse_dictionary(int argc, char **argv, char ***dictionary_ptr,
        long *dictionary_length);

/* This function parses command line arguments
 * and saves information into args_t struct */
void init_args(int argc, char **argv, args_t *args)
{
    int opt;

    bool valid_usage;
    bool filename_flag;

    char *filename;

    char **dictionary;
    long dictionary_length;

    /* Data structure required for the correct work of getopt_long */
    enum {
        FILENAME_VAL, UNDEFINED_VAL
    };

    struct option longopts[] = {
        {"filename", required_argument, NULL, FILENAME_VAL},
        {NULL, no_argument, NULL, UNDEFINED_VAL}
    };

#ifdef DEBUG
    if (logfile) {
        fprintf(logfile, "The program started parsing arguments.\n");
    }
#endif

    /* Starting values setting */
    valid_usage = true;
    filename_flag = false;
    filename = NULL;

    /* Command line arguments parsing */
    while ((opt = getopt_long(argc, argv, ":f:", longopts, NULL)) != -1) {
        switch (opt) {
            case FILENAME_VAL:
            case 'f':
                /* User specified filename */
                if (filename_flag) {
                    /* User specified filename more than once.
                     * Invalid usage */
#ifdef DEBUG
                    if (logfile) {
                        fprintf(logfile, "The user specified a file more than "
                                "once. Invalid usage detected.\n");
                    }
#endif
                    valid_usage = false;
                    filename = NULL;
                    break;
                }
                filename_flag = true;
                filename = optarg;
#ifdef DEBUG
                if (logfile) {
                    fprintf(logfile, "The user specified a file name: %s.\n",
                            filename);
                }
#endif
                break;
            default:
                /* Something went wrong while parsing command line arguments.
                 * Invalid usage */
#ifdef DEBUG
                if (logfile) {
                    fprintf(logfile, "The user submitted an undefined option. "
                            "Invalid usage detected.\n");
                }
#endif
                valid_usage = false;
                break;
        }
    }

    if (!filename_flag) {
        /* User didn't specify essential data */
#ifdef DEBUG
        if (logfile) {
            fprintf(logfile, "The user did not specify a file name. "
                    "Invalid usage detected.\n");
        }
#endif
        valid_usage = false;
    }

    /* Parsing dictionary for searching */
    parse_dictionary(argc, argv, &dictionary, &dictionary_length);

    /* Saving parsing results into args_t struct */
    args->valid_usage = valid_usage;
    args->filename = filename;
    args->dictionary = dictionary;
    args->dictionary_length = dictionary_length;

#ifdef DEBUG
    if (logfile) {
        fprintf(logfile, "Dictionary length: %ld.\n", dictionary_length);
        fprintf(logfile, "Usage: %s.\n", valid_usage ? "valid" : "invalid");
    }
#endif
}

/* This function frees allocated memory in args_t variable */
void destroy_args(args_t *args)
{
    if (args->dictionary) {
        free(args->dictionary);
    }

#ifdef DEBUG
    if (logfile) {
        fprintf(logfile, "Memory allocated for dictionary pointers was freed.\n");
    }
#endif
}

/* This function is used for extracting dictionary entities from
 * command line arguments. */
static void parse_dictionary(int argc, char **argv, char ***dictionary_ptr,
        long *dictionary_length)
{
    char **dictionary;
    long length;
    int i;

    /* Starting valued setting */
    length = 0;
    dictionary = NULL;

    i = 1;
    while (i < argc) {
        if (argv[i][0] != '-') {
            /* The element is not an option therefore it is dictionary entity */
            length++;
            dictionary = realloc(dictionary, sizeof(char*) * length);
            dictionary[length - 1] = argv[i];
#ifdef DEBUG
            if (logfile) {
                fprintf(logfile, "Dictionary entity received: %s.\n",
                        dictionary[length - 1]);
            }
#endif
            i++;
        } else {
            /* The element is an option. Skipping this and the next one */
            i += 2;
        }
    }

    /* Saving extracted information */
    *dictionary_ptr = dictionary;
    *dictionary_length = length;
}
