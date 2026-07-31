#include <stdlib.h>
#include <stdio.h>

#include "stack_utils.h"
#include "read_data.h"
#include "introsort.h"
#include "stack.h"
#include "codes.h"
#include "args.h"

#define DEFAULT_FILENAME "numbers.txt"

int main(int argc, char **argv) { // 156 + 1160n^2 + 446n + 272log(n) + 1204n^2log(n)
    char *filename;
    stack_t stack;
    code_t result;
    args_t args;

    parse_args(argc, argv, &args); // (33)

    filename = get_filename(&args, DEFAULT_FILENAME); // (5)

    init(&stack); // (4)

    result = read_data(&stack, filename); // 4n + 32

    if (result == success_code) { // (1)
        /*
         * Success
         */
#       ifdef DEBUG
        fprintf(stderr, "Data was succesfully read from file.\n");
#       endif
    } else if (result == open_file_error_code) { // (1)
        /*
         * Couldn't open file
         */
#       ifdef DEBUG
        fprintf(stderr, "Couldn't open file %s.\n", filename);
#       endif

        printf("Couldn't open file %s.\n", filename); // (1)

#       ifdef DEBUG
        fprintf(stderr, "Program terminated.\n");
#       endif

        exit(open_file_error_code); // (1)
    } else if (result == file_format_error_code) { // (1)
        /*
         * Invalid file format
         */
#       ifdef DEBUG
        fprintf(stderr, "%s has invalid file format.\n", filename);
#       endif
        printf("%s has invalid file format.\n", filename); // (1)
#       ifdef DEBUG
        fprintf(stderr, "Program terminated.\n");
#       endif
        exit(file_format_error_code); // (1)
    }

    introsort(&stack); // 64 + 1160n^2 + 337n + 272log(n) + 1204n^2log(n)
    print_data(&stack); // 99n + 7
    erase(&stack); // 6n + 9

    return 0; // (1)
}
