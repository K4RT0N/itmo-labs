#include <stdlib.h>
#include <stdio.h>

#include "introsort.h"
#include "read_data.h"
#include "messages.h"
#include "args.h"

#define DEFAULT_FILENAME "numbers.txt"
#define STATIC_ARRAY_SIZE 512
#define DYNAMIC_ARRAY_STARTING_SIZE 32

int main(int argc, char **argv)
{
    double static_array[STATIC_ARRAY_SIZE];
    double *array;
    
    char *filename;
    code_t result;
    bool dynamic;
    args_t args;
    int length;

#   ifdef DEBUG
    fprintf(stderr, "The main function started.\n");
#   endif

    parse_args(argc, argv, &args);
    if (!is_correct(&args)) {
#       ifdef DEBUG
        fprintf(stderr, "Invalid usage detected.\n");
#       endif

        usage_message(argv[0]);

#       ifdef DEBUG
        fprintf(stderr, "Program terminated.\n");
#       endif

        exit(0);
    }

    filename = get_filename(&args, DEFAULT_FILENAME);

    if (is_static(&args)) {
        array = static_array;
        length = STATIC_ARRAY_SIZE;
#       ifdef DEBUG
        fprintf(stderr, "The static array was chosen. "
                        "The array length is %d.\n", length);
#       endif
        dynamic = false;
    } else {
        array = malloc(sizeof(double) * DYNAMIC_ARRAY_STARTING_SIZE);
        length = DYNAMIC_ARRAY_STARTING_SIZE;
#       ifdef DEBUG
        fprintf(stderr, "The dynamic array was chosen. "
                        "The array length is %d.\n", length);
#       endif
        dynamic = true;
    }

    result = read_data(&array, &length, filename, dynamic);
    switch (result) {
        case success_code:
#           ifdef DEBUG
            fprintf(stderr, "Data was succesfully read from file.\n");
#           endif
            break;
        case open_file_error_code:
#           ifdef DEBUG
            fprintf(stderr, "Couldn't open file %s.\n", filename);
#           endif

            printf("Couldn't open file %s.\n", filename);

#           ifdef DEBUG
            fprintf(stderr, "Program terminated.\n");
#           endif

            exit(open_file_error_code);
            break;
        case file_format_error_code:
#           ifdef DEBUG
            fprintf(stderr, "%s has invalid file format.\n", filename);
#           endif

            printf("%s has invalid file format.\n", filename);

#           ifdef DEBUG
            fprintf(stderr, "Program terminated.\n");
#           endif
            
            exit(file_format_error_code);
            break;
        case no_space_error_code:
#           ifdef DEBUG
            fprintf(stderr, "Program ran out of space writing in "
                            "static array.\n");
#           endif

            printf("Program ran out of space writing in static array.\n");

#           ifdef DEBUG
            fprintf(stderr, "Program terminated.\n");
#           endif

            exit(no_space_error_code);
            break;
    }

    introsort(array, length);

    for (int i = 0; i < length; i++) {
        printf("%lf\n", array[i]);
    }
    
    if (is_dynamic(&args)) {
        free(array);
    }

#   ifdef DEBUG
    fprintf(stderr, "The main function ended.\n");
#   endif

    return 0;
}
