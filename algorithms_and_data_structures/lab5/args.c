#include <getopt.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

#include "args.h"

#ifdef DEBUG
#include "logfile.h"
#endif

/*
 * Parses the command line and writes the resulting configuration to args.
 *
 * The solver accepts exactly one input-file option and at most one output-file
 * option.  Keeping these checks here prevents a later stage from working with
 * an ambiguous set of filenames.
 */
void init_args(int argc, char **argv, args_t *args)
{
    int opt;
    bool valid_usage;
    bool filename_flag;
    bool output_flag;

    /* Values used by getopt_long for long options without a short alias. */
    enum {
        FILENAME_VAL,
        OUTPUT_VAL,
        UNDEFINED_VAL
    };

    /* Long and short spellings are intentionally handled by the same cases. */
    struct option longopts[] = {
        {"file", required_argument, NULL, FILENAME_VAL},
        {"output", required_argument, NULL, OUTPUT_VAL},
        {NULL, no_argument, NULL, UNDEFINED_VAL}
    };

#ifdef DEBUG
    if (logfile) {
        fprintf(logfile, "The program started parsing arguments.\n");
    }
#endif

    valid_usage = true;
    filename_flag = false;
    output_flag = false;

    args->filename = NULL;
    args->output_filename = "solution.csv";

    /*
     * A leading colon makes getopt_long return ':' for a missing argument.
     * All malformed variants are handled by the default branch below.
     */
    while ((opt = getopt_long(argc, argv, ":f:o:", longopts, NULL)) != -1) {
        switch (opt) {
            case FILENAME_VAL:
            case 'f':
                if (filename_flag) {
                    /* Multiple input files make usage ambiguous. */
                    valid_usage = false;
                    break;
                }
                filename_flag = true;
                args->filename = optarg;
                break;
            case OUTPUT_VAL:
            case 'o':
                if (output_flag) {
                    /* Multiple output names make usage ambiguous. */
                    valid_usage = false;
                    break;
                }
                output_flag = true;
                args->output_filename = optarg;
                break;
            default:
                /* getopt_long reports all malformed and unknown options here. */
                valid_usage = false;
                break;
        }
    }

    if (!filename_flag || optind != argc) {
        /* The problem file is required and positional arguments are unsupported. */
        valid_usage = false;
    }

    args->valid_usage = valid_usage;

#ifdef DEBUG
    if (logfile) {
        fprintf(logfile, "Usage: %s.\n", valid_usage ? "valid" : "invalid");
    }
#endif
}
