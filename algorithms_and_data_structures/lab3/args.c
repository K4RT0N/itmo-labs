#include <stdbool.h>
#include <errno.h>
#include <getopt.h>
#include <limits.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "args.h"

#ifdef DEBUG
#include "logfile.h"
#endif

/*
 * Extracts non-option command-line arguments into a newly allocated
 * dictionary pointer array.  The strings themselves are not copied because
 * argv remains valid throughout main.  The function returns false only when
 * the pointer array cannot be enlarged safely.
 */
static bool parse_dictionary(int argc, char **argv, char ***dictionary_ptr,
        long *dictionary_length);

/*
 * Parses command-line options and records the input-file name, the capacity
 * of the circular queue and the remaining dictionary entries.  Parsing errors
 * are remembered in valid_usage instead of terminating here, allowing main
 * to print the common usage message.
 */
void init_args(int argc, char **argv, args_t *args)
{
    int opt;

    bool valid_usage;
    bool filename_flag;
    bool cqueue_length_flag;
    bool dictionary_parsed;

    char *filename;
    char *endptr;

    char **dictionary;
    long dictionary_length;

    long cqueue_length;

    /*
     * Numeric values returned by getopt_long for long options.  They are
     * distinct from the short-option characters handled in the same switch.
     */
    enum {
        FILENAME_VAL, LENGTH_VAL, UNDEFINED_VAL
    };

    /*
     * Long options mirror -f/--filename and -n/--length.  The terminating
     * entry tells getopt_long where the table ends.
     */
    struct option longopts[] = {
        {"filename", required_argument, NULL, FILENAME_VAL},
        {"length", required_argument, NULL, LENGTH_VAL},
        {NULL, no_argument, NULL, UNDEFINED_VAL}
    };

#ifdef DEBUG
    if (logfile) {
        fprintf(logfile, "The program started parsing arguments.\n");
    }
#endif

    /*
     * Initialise every local result before getopt_long starts.  The flags
     * distinguish an absent option from an option whose text happens to
     * produce the same value as a default.
     */
    valid_usage = true;
    filename_flag = false;
    cqueue_length_flag = false;
    filename = NULL;
    cqueue_length = -1;
    dictionary = NULL;
    dictionary_length = 0;

    /*
     * getopt_long returns one option at a time.  The leading ':' makes a
     * missing option argument reach the default branch instead of printing
     * its own diagnostic message.
     */
    while ((opt = getopt_long(argc, argv, ":f:n:", longopts, NULL)) != -1) {
        switch (opt) {
            case FILENAME_VAL:
            case 'f':
                /* The input file name may be supplied exactly once. */
                if (filename_flag) {
                    valid_usage = false;
                    filename = NULL;
#ifdef DEBUG
                    if (logfile) {
                        fprintf(logfile, "The user specified filename more "
                                "than once. Usage is invalid.\n");
                    }
#endif
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

            case LENGTH_VAL:
            case 'n':
                /* The queue capacity may be supplied exactly once. */
                if (cqueue_length_flag) {
                    valid_usage = false;
                    cqueue_length = -1;
#ifdef DEBUG
                    if (logfile) {
                        fprintf(logfile, "The user specified queue length "
                                "more than once. Usage is invalid.\n");
                    }
#endif
                    break;
                }

                cqueue_length_flag = true;

                /*
                 * strtol reports both syntax errors through endptr and range
                 * errors through errno.  A strictly positive integer is the
                 * only capacity accepted by this program.
                 */
                errno = 0;
                cqueue_length = strtol(optarg, &endptr, 10);
                if (optarg == endptr || *endptr != '\0'
                        || errno == ERANGE || cqueue_length <= 0) {
                    valid_usage = false;
                    cqueue_length = -1;
#ifdef DEBUG
                    if (logfile) {
                        fprintf(logfile, "The queue length \"%s\" is invalid.\n",
                                optarg);
                    }
#endif
                    break;
                }

#ifdef DEBUG
                if (logfile) {
                    fprintf(logfile, "The user specified queue length: %ld.\n",
                            cqueue_length);
                }
#endif
                break;

            default:
                /*
                 * getopt_long reaches this branch for an unknown option or a
                 * missing required option value.  The final usage message is
                 * deliberately emitted by main, not by this low-level parser.
                 */
                valid_usage = false;
#ifdef DEBUG
                if (logfile) {
                    fprintf(logfile, "An undefined or incomplete option was "
                            "received. Usage is invalid.\n");
                }
#endif
                break;
        }
    }

    /*
     * Both the file name and the queue capacity are mandatory.  The parser
     * keeps checking remaining arguments even after an earlier error so that
     * the structure is always safe to destroy.
     */
    if (!(filename_flag && cqueue_length_flag)) {
        valid_usage = false;
#ifdef DEBUG
        if (logfile) {
            fprintf(logfile, "Not every mandatory option was specified.\n");
        }
#endif
    }

    /* Remaining non-option words form the dictionary used by Aho-Corasick. */
    dictionary_parsed = parse_dictionary(argc, argv, &dictionary,
            &dictionary_length);
    if (!dictionary_parsed || dictionary_length > INT_MAX) {
        valid_usage = false;
        free(dictionary);
        dictionary = NULL;
        dictionary_length = 0;
#ifdef DEBUG
        if (logfile) {
            fprintf(logfile, "Dictionary allocation or size validation "
                    "failed. Usage is invalid.\n");
        }
#endif
    }

    /*
     * Copy the complete parsing result to args only after all local checks
     * have finished.  This gives callers a single coherent configuration.
     */
    args->cqueue_length = cqueue_length;
    args->valid_usage = valid_usage;
    args->filename = filename;
    args->dictionary = dictionary;
    args->dictionary_length = (int)dictionary_length;

#ifdef DEBUG
    if (logfile) {
        fprintf(logfile, "Argument parsing ended. Usage: %s. Dictionary "
                "entries: %d.\n", args->valid_usage ? "valid" : "invalid",
                args->dictionary_length);
    }
#endif
}

/*
 * Releases only the array allocated by parse_dictionary.  Every pointed-to
 * string belongs to argv and therefore must not be freed here.
 */
void destroy_args(args_t *args)
{
    if (args->dictionary) {
        free(args->dictionary);
        args->dictionary = NULL;
    }

    args->dictionary_length = 0;

#ifdef DEBUG
    if (logfile) {
        fprintf(logfile, "Argument resources were released.\n");
    }
#endif
}

/*
 * Collects arguments that are not options.  This follows the original
 * command-line convention: an option and its following value are skipped,
 * while every other word is treated as a dictionary entity.
 */
static bool parse_dictionary(int argc, char **argv, char ***dictionary_ptr,
        long *dictionary_length)
{
    char **dictionary;
    char **resized_dictionary;
    long length;
    int i;

    dictionary = NULL;
    length = 0;
    i = 1;

    while (i < argc) {
        if (argv[i][0] != '-') {
            /*
             * This word is not an option token, so preserve the pointer as a
             * search pattern.  realloc may move the array; use a temporary
             * pointer to avoid losing the old allocation on failure.
             */
            if (length == LONG_MAX) {
                free(dictionary);
                return false;
            }

            resized_dictionary = realloc(dictionary,
                    sizeof(char *) * (size_t)(length + 1));
            if (resized_dictionary == NULL) {
                free(dictionary);
#ifdef DEBUG
                if (logfile) {
                    fprintf(logfile, "Unable to enlarge the dictionary "
                            "pointer array.\n");
                }
#endif
                return false;
            }

            dictionary = resized_dictionary;
            dictionary[length] = argv[i];
            length++;
#ifdef DEBUG
            if (logfile) {
                fprintf(logfile, "Dictionary entity accepted: %s.\n", argv[i]);
            }
#endif
            i++;
        } else {
            /*
             * Options used by this program always consume their next token.
             * Keep the historical parsing rule so existing invocations retain
             * the same dictionary contents.
             */
            i += 2;
        }
    }

    *dictionary_ptr = dictionary;
    *dictionary_length = length;
    return true;
}
